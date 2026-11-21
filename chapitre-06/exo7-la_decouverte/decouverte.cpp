// Chapitre 6 — Exercice 7 : la découverte.
//   decouverte serveur "Salle de Patrick"   diffuse toutes les deux secondes
//   decouverte client                       écoute six secondes et liste
#include <cstdio>
#include <cstring>
#include <new>

#include "NKNetwork/NKNetwork.h"
#include "NKTime/NkChrono.h"

using namespace nkentseu;
using namespace nkentseu::net;

static int Serveur(const char *nom) {
    NkDiscovery::ServerInfo info;
    std::strncpy(info.name, nom, sizeof(info.name) - 1);
    std::strncpy(info.gameMode, "essai", sizeof(info.gameMode) - 1);
    // maxPlayers vaut 0 par défaut : une telle annonce est écartée par Listen, sans message.
    info.maxPlayers = 4;
    info.playerCount = 1;
    for (int i = 0; i < 30; ++i) {
        NkNetResult r = NkDiscovery::Broadcast(info);
        std::printf("[serveur] diffusion %d : %s\n", i + 1, r == NkNetResult::NK_NET_OK ? "OK" : "ECHEC");
        NkChrono::SleepMilliseconds(2000);
    }
    return 0;
}

static int Client() {
    NkVector<NkDiscovery::ServerInfo> trouves;
    // Bloque six secondes : voulu ici, jamais dans une boucle de jeu.
    NkNetResult r = NkDiscovery::Listen(6000, trouves);
    if (r != NkNetResult::NK_NET_OK) {
        std::printf("[client] Listen a echoue\n");
        return 1;
    }
    std::printf("[client] %u serveur(s) trouve(s)\n", static_cast<unsigned>(trouves.Size()));
    for (const NkDiscovery::ServerInfo &s : trouves) {
        std::printf("  %s  %s  (%u/%u joueurs)\n", s.addr.ToString().CStr(), s.name, s.playerCount, s.maxPlayers);
    }
    return 0;
}

int main(int argc, char **argv) {
    // Sortie non tamponnée : les lignes restent lisibles même si le programme est arrêté de force.
    std::setvbuf(stdout, nullptr, _IONBF, 0);
    NkSocket::PlatformInit();
    if (argc > 1 && std::strcmp(argv[1], "serveur") == 0) {
        return Serveur(argc > 2 ? argv[2] : "Serveur Nkentseu");
    }
    return Client();
}
