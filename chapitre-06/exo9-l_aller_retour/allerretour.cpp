// Chapitre 6 — Exercice 9 : l'aller-retour.
//   allerretour serveur              renvoie chaque message à son expéditeur
//   allerretour client 127.0.0.1     cent allers-retours, un message à la fois
// Le client écrit mesures-rtt.txt (une mesure par ligne) et affiche le bilan.
#include <cstdio>
#include <cstring>
#include <new>

#include "NKCore/NkAtomic.h"
#include "NKNetwork/NKNetwork.h"
#include "NKTime/NkChrono.h"

using namespace nkentseu;
using namespace nkentseu::net;

namespace {
    NkAtomic<nk_uint64> gPair(0);
}

static int Serveur() {
    NkConnectionManager reseau;
    if (reseau.StartServer(7777, 8) != NkNetResult::NK_NET_OK) {
        return 1;
    }
    NkVector<NkReceiveMsg> recus;
    NkChrono chrono;
    while (chrono.Elapsed().seconds < 60.0) {
        recus.Clear();
        reseau.DrainAll(recus);
        for (const NkReceiveMsg &m : recus) {
            // Renvoi tel quel, au champ from du message reçu.
            (void)reseau.SendTo(m.from, m.data, m.size);
        }
        NkChrono::SleepMilliseconds(1);
    }
    reseau.Shutdown();
    return 0;
}

static int Client(const char *hote) {
    NkConnectionManager reseau;
    reseau.onPeerConnected = [](NkPeerId p) {
        gPair.Store(p.value);
    };
    if (reseau.Connect(NkAddress(hote, 7777)) != NkNetResult::NK_NET_OK) {
        return 1;
    }
    NkChrono attente;
    while (gPair.Load() == 0 && attente.Elapsed().seconds < 5.0) {
        NkChrono::SleepMilliseconds(5);
    }
    if (gPair.Load() == 0) {
        std::printf("[client] pas de connexion\n");
        return 1;
    }
    NkPeerId serveur;
    serveur.value = gPair.Load();

    constexpr int kN = 100;
    double mesures[kN];
    NkVector<NkReceiveMsg> recus;
    for (uint32 numero = 0; numero < kN; ++numero) {
        uint8 envoi[4];
        std::memcpy(envoi, &numero, 4);
        NkChrono chrono;
        (void)reseau.SendTo(serveur, envoi, 4);
        // Un seul message en vol : on attend le retour de CE numéro avant le suivant.
        bool revenu = false;
        while (!revenu && chrono.Elapsed().seconds < 5.0) {
            recus.Clear();
            reseau.DrainAll(recus);
            for (const NkReceiveMsg &m : recus) {
                uint32 lu = 0;
                if (m.size == 4) {
                    std::memcpy(&lu, m.data, 4);
                }
                if (lu == numero) {
                    revenu = true;
                }
            }
        }
        mesures[numero] = chrono.Elapsed().milliseconds;
    }

    FILE *f = std::fopen("mesures-rtt.txt", "w");
    double somme = 0.0;
    double mini = mesures[0];
    double maxi = mesures[0];
    // Lissage du moteur : SRTT part de 100 ms, alpha = 0,125.
    double srtt = 100.0;
    for (int i = 0; i < kN; ++i) {
        if (f != nullptr) {
            std::fprintf(f, "%.3f\n", mesures[i]);
        }
        somme += mesures[i];
        if (mesures[i] < mini) {
            mini = mesures[i];
        }
        if (mesures[i] > maxi) {
            maxi = mesures[i];
        }
        srtt = (1.0 - 0.125) * srtt + 0.125 * mesures[i];
    }
    if (f != nullptr) {
        std::fclose(f);
    }

    NkConnectionStats stats;
    bool ok = reseau.GetConnectionStats(serveur, stats);
    std::printf("Moyenne : %.3f\n", somme / kN);
    std::printf("Minimum : %.3f\n", mini);
    std::printf("Maximum : %.3f\n", maxi);
    std::printf("Lissee : %.3f\n", srtt);
    std::printf("Moteur : %.3f%s\n", ok ? stats.rttMs : 0.f, ok ? "" : " (stats indisponibles)");
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
    return Client(argc > 2 ? argv[2] : "127.0.0.1");
}
