// Chapitre 6 — Exercice 10 : le joueur fantôme.
//   fantome serveur                    tient la table des joueurs
//   fantome client 127.0.0.1           envoie toutes les 100 ms, jusqu'à ce qu'on le tue
//   fantome client 127.0.0.1 propre    envoie 3 s, puis Shutdown() avant de quitter
//
// onPeerConnected et onPeerDisconnected sont appelés par le FIL RÉSEAU du moteur
// (NkConnectionManager::NetThreadLoop), pas par la boucle : la table est donc
// partagée entre deux fils, et protégée par un verrou.
#include <cstdio>
#include <cstring>
#include <new>

#include "NKCore/NkAtomic.h"
#include "NKNetwork/NKNetwork.h"
#include "NKThreading/NkMutex.h"
#include "NKThreading/NkScopedLock.h"
#include "NKTime/NkChrono.h"

using namespace nkentseu;
using namespace nkentseu::net;
using nkentseu::threading::NkMutex;
// NkScopedLock : alias nkentseu::NkScopedLock, déjà visible par « using namespace nkentseu ».

namespace {
    struct Joueur {
        nk_uint64 pair = 0;
        double dernierMessageMs = 0.0;
        bool present = false;
    };
    constexpr int kMaxJoueurs = 16;
    Joueur gTable[kMaxJoueurs];
    NkMutex gVerrou;
    NkChrono gHorloge;

    // À appeler verrou tenu.
    void AfficherTable(const char *quand) {
        std::printf("[serveur] table (%s) :", quand);
        int n = 0;
        for (const Joueur &j : gTable) {
            if (j.present) {
                std::printf(" %llu", static_cast<unsigned long long>(j.pair));
                ++n;
            }
        }
        std::printf("%s\n", n == 0 ? " vide" : "");
    }

    NkAtomic<nk_uint64> gPair(0);
}

static int Serveur() {
    NkConnectionManager reseau;
    reseau.onPeerConnected = [](NkPeerId p) {
        NkScopedLock<NkMutex> verrou(gVerrou);
        for (Joueur &j : gTable) {
            if (!j.present) {
                j.present = true;
                j.pair = p.value;
                j.dernierMessageMs = gHorloge.Elapsed().milliseconds;
                break;
            }
        }
        AfficherTable("connexion");
    };
    reseau.onPeerDisconnected = [](NkPeerId p, const char *raison) {
        NkScopedLock<NkMutex> verrou(gVerrou);
        double maintenant = gHorloge.Elapsed().milliseconds;
        for (Joueur &j : gTable) {
            if (j.present && j.pair == p.value) {
                std::printf("[serveur] deconnexion de %llu, raison : %s, %.0f ms apres son dernier message\n",
                            static_cast<unsigned long long>(p.value), raison != nullptr ? raison : "(aucune)",
                            maintenant - j.dernierMessageMs);
                j.present = false;
            }
        }
        AfficherTable("deconnexion");
    };
    if (reseau.StartServer(7777, 8) != NkNetResult::NK_NET_OK) {
        return 1;
    }
    NkVector<NkReceiveMsg> recus;
    while (gHorloge.Elapsed().seconds < 300.0) {
        recus.Clear();
        reseau.DrainAll(recus);
        if (!recus.Empty()) {
            NkScopedLock<NkMutex> verrou(gVerrou);
            double maintenant = gHorloge.Elapsed().milliseconds;
            for (const NkReceiveMsg &m : recus) {
                for (Joueur &j : gTable) {
                    if (j.present && j.pair == m.from.value) {
                        j.dernierMessageMs = maintenant;
                    }
                }
            }
        }
        NkChrono::SleepMilliseconds(10);
    }
    reseau.Shutdown();
    return 0;
}

static int Client(const char *hote, bool propre) {
    NkConnectionManager reseau;
    reseau.onPeerConnected = [](NkPeerId p) {
        gPair.Store(p.value);
    };
    if (reseau.Connect(NkAddress(hote, 7777)) != NkNetResult::NK_NET_OK) {
        return 1;
    }
    NkChrono attente;
    while (gPair.Load() == 0 && attente.Elapsed().seconds < 5.0) {
        NkChrono::SleepMilliseconds(10);
    }
    if (gPair.Load() == 0) {
        std::printf("[client] pas de connexion\n");
        return 1;
    }
    NkPeerId serveur;
    serveur.value = gPair.Load();
    NkChrono duree;
    const char *texte = "present";
    while (!propre || duree.Elapsed().seconds < 3.0) {
        (void)reseau.SendTo(serveur, reinterpret_cast<const uint8 *>(texte), 7);
        NkChrono::SleepMilliseconds(100);
    }
    std::printf("[client] fermeture propre : Shutdown()\n");
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
    bool propre = (argc > 3 && std::strcmp(argv[3], "propre") == 0);
    return Client(hote, propre);
}
