// Chapitre 6 — Exercice 6 : deux machines se parlent.
// Un seul programme, deux rôles choisis par argument :
//   parle serveur
//   parle client 192.168.1.20      (adresse du serveur ; 127.0.0.1 sur une seule machine)
#include <cstdio>
#include <cstring>
// NkFunction (tiré par NKNetwork) fait un placement new sans inclure <new> :
// le moteur l'obtient par son en-tête précompilé, un programme seul doit l'inclure.
#include <new>

#include "NKCore/NkAtomic.h"
#include "NKNetwork/NKNetwork.h"
#include "NKTime/NkChrono.h"

using namespace nkentseu;
using namespace nkentseu::net;

namespace {
    // Écrit par le fil réseau du moteur (rappel), lu par la boucle : atomique.
    NkAtomic<nk_uint64> gPairServeur(0);
}

static int Serveur() {
    NkConnectionManager reseau;
    reseau.onPeerConnected = [](NkPeerId pair) {
        std::printf("[serveur] pair connecte : %llu\n", static_cast<unsigned long long>(pair.value));
    };
    if (reseau.StartServer(7777, 8) != NkNetResult::NK_NET_OK) {
        std::printf("[serveur] StartServer a echoue (port 7777 deja pris ?)\n");
        return 1;
    }
    std::printf("[serveur] en ecoute sur le port 7777\n");

    NkVector<NkReceiveMsg> recus;
    NkChrono chrono;
    // Trente secondes d'écoute, puis arrêt propre.
    while (chrono.Elapsed().seconds < 30.0) {
        // Sans DrainAll à chaque tour, les messages restent en file.
        recus.Clear();
        reseau.DrainAll(recus);
        for (const NkReceiveMsg &m : recus) {
            std::printf("[serveur] message de %llu : %.*s\n", static_cast<unsigned long long>(m.from.value),
                        static_cast<int>(m.size), reinterpret_cast<const char *>(m.data));
        }
        NkChrono::SleepMilliseconds(10);
    }
    reseau.Shutdown();
    return 0;
}

static int Client(const char *hote) {
    NkConnectionManager reseau;
    reseau.onPeerConnected = [](NkPeerId pair) {
        std::printf("[client] pair connecte : %llu\n", static_cast<unsigned long long>(pair.value));
        gPairServeur.Store(pair.value);
    };
    if (reseau.Connect(NkAddress(hote, 7777)) != NkNetResult::NK_NET_OK) {
        std::printf("[client] Connect a echoue\n");
        return 1;
    }
    // La connexion se fait dans le fil du moteur : on attend qu'elle soit établie.
    NkChrono attente;
    while (gPairServeur.Load() == 0 && attente.Elapsed().seconds < 5.0) {
        NkChrono::SleepMilliseconds(10);
    }
    if (gPairServeur.Load() == 0) {
        std::printf("[client] pas de connexion apres 5 s (adresse ? pare-feu ?)\n");
        return 1;
    }
    NkPeerId serveur;
    serveur.value = gPairServeur.Load();
    const char *texte = "Bonjour depuis le client";
    NkNetResult r = reseau.SendTo(serveur, reinterpret_cast<const uint8 *>(texte),
                                  static_cast<uint32>(std::strlen(texte)));
    std::printf("[client] message envoye a %llu : %s (%s)\n", static_cast<unsigned long long>(serveur.value), texte,
                r == NkNetResult::NK_NET_OK ? "OK" : "ECHEC");
    // On laisse au fil réseau le temps d'émettre avant de couper.
    NkChrono::SleepMilliseconds(1000);
    reseau.Shutdown();
    return 0;
}

int main(int argc, char **argv) {
    // Sortie non tamponnée : les lignes restent lisibles même si le programme est arrêté de force.
    std::setvbuf(stdout, nullptr, _IONBF, 0);
    NkSocket::PlatformInit();
    if (argc > 1 && std::strcmp(argv[1], "serveur") == 0) {
        return Serveur();
    }
    const char *hote = (argc > 2) ? argv[2] : "127.0.0.1";
    return Client(hote);
}
