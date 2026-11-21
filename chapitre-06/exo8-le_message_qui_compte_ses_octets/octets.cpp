// Chapitre 6 — Exercice 8 : le message qui compte ses octets.
//   octets recepteur               attend les deux paquets et les relit
//   octets emetteur 127.0.0.1      écrit, mesure et envoie les deux paquets
#include <cmath>
#include <cstdio>
#include <cstring>
#include <new>

#include "NKCore/NkAtomic.h"
#include "NKNetwork/NKNetwork.h"
#include "NKTime/NkChrono.h"

using namespace nkentseu;
using namespace nkentseu::net;

namespace {
    // L'état choisi. Le récepteur connaît les mêmes valeurs pour mesurer l'écart.
    struct Etat {
        bool vivant;
        uint8 numero;
        float32 x;
        float32 y;
        float32 angle;
        int32 sante;
    };
    const Etat kEtat = {true, 42, 123.456f, -321.987f, 271.3f, 87};

    // Bornes et précisions : identiques à l'écriture et à la relecture, dans le même ordre.
    constexpr float32 kPosMin = -500.f;
    constexpr float32 kPosMax = 500.f;
    constexpr float32 kPosPrec = 0.01f;  // le centimètre
    constexpr float32 kAngMin = 0.f;
    constexpr float32 kAngMax = 360.f;
    constexpr float32 kAngPrec = 0.5f;  // le demi-degré

    NkAtomic<nk_uint64> gPair(0);

    uint32 EcrireComplet(uint8 *buf, uint32 cap) {
        NkBitWriter w(buf, cap);
        w.WriteU8(kEtat.vivant ? 1 : 0);
        w.WriteU8(kEtat.numero);
        w.WriteF32(kEtat.x);
        w.WriteF32(kEtat.y);
        w.WriteF32(kEtat.angle);
        w.WriteI32(kEtat.sante);
        std::printf("complet  : %u octets, %u bits%s\n", w.BytesWritten(), w.BitsWritten(),
                    w.IsOverflowed() ? " (DEBORDEMENT)" : "");
        return w.BytesWritten();
    }

    uint32 EcrireQuantifie(uint8 *buf, uint32 cap) {
        NkBitWriter w(buf, cap);
        w.WriteBool(kEtat.vivant);
        w.WriteU8(kEtat.numero);
        w.WriteF32Q(kEtat.x, kPosMin, kPosMax, kPosPrec);
        w.WriteF32Q(kEtat.y, kPosMin, kPosMax, kPosPrec);
        w.WriteF32Q(kEtat.angle, kAngMin, kAngMax, kAngPrec);
        w.WriteInt(kEtat.sante, 0, 100);
        std::printf("quantifie : %u octets, %u bits%s\n", w.BytesWritten(), w.BitsWritten(),
                    w.IsOverflowed() ? " (DEBORDEMENT)" : "");
        return w.BytesWritten();
    }

    void Relire(const NkReceiveMsg &m) {
        NkBitReader r(m.data, m.size);
        Etat lu{};
        // Le premier octet du message dit lequel des deux paquets suit.
        uint8 sorte = r.ReadU8();
        if (sorte == 0) {
            lu.vivant = r.ReadU8() != 0;
            lu.numero = r.ReadU8();
            lu.x = r.ReadF32();
            lu.y = r.ReadF32();
            lu.angle = r.ReadF32();
            lu.sante = r.ReadI32();
        } else {
            lu.vivant = r.ReadBool();
            lu.numero = r.ReadU8();
            lu.x = r.ReadF32Q(kPosMin, kPosMax, kPosPrec);
            lu.y = r.ReadF32Q(kPosMin, kPosMax, kPosPrec);
            lu.angle = r.ReadF32Q(kAngMin, kAngMax, kAngPrec);
            lu.sante = r.ReadInt(0, 100);
        }
        std::printf("[%s] relu : vivant %d, numero %u, x %.4f, y %.4f, angle %.4f, sante %d%s\n",
                    sorte == 0 ? "complet" : "quantifie", lu.vivant ? 1 : 0, lu.numero, lu.x, lu.y, lu.angle,
                    lu.sante, r.IsOverflowed() ? " (DEBORDEMENT)" : "");
        std::printf("[%s] ecarts : vivant %d, numero %d, x %.4f m, y %.4f m, angle %.4f deg, sante %d\n",
                    sorte == 0 ? "complet" : "quantifie", lu.vivant != kEtat.vivant ? 1 : 0,
                    static_cast<int>(lu.numero) - static_cast<int>(kEtat.numero), std::fabs(lu.x - kEtat.x),
                    std::fabs(lu.y - kEtat.y), std::fabs(lu.angle - kEtat.angle), lu.sante - kEtat.sante);
    }
}

static int Recepteur() {
    NkConnectionManager reseau;
    reseau.onPeerConnected = [](NkPeerId p) {
        std::printf("[recepteur] pair connecte : %llu\n", static_cast<unsigned long long>(p.value));
    };
    if (reseau.StartServer(7777, 8) != NkNetResult::NK_NET_OK) {
        std::printf("[recepteur] StartServer a echoue\n");
        return 1;
    }
    NkVector<NkReceiveMsg> recus;
    int lus = 0;
    NkChrono chrono;
    while (lus < 2 && chrono.Elapsed().seconds < 30.0) {
        recus.Clear();
        reseau.DrainAll(recus);
        for (const NkReceiveMsg &m : recus) {
            std::printf("[recepteur] message de %u octets\n", m.size);
            Relire(m);
            ++lus;
        }
        NkChrono::SleepMilliseconds(10);
    }
    reseau.Shutdown();
    return 0;
}

static int Emetteur(const char *hote) {
    // Les paquets mesurés, sans l'octet d'aiguillage.
    uint8 complet[64];
    uint8 quantifie[64];
    uint32 tc = EcrireComplet(complet, sizeof(complet));
    uint32 tq = EcrireQuantifie(quantifie, sizeof(quantifie));

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
        std::printf("[emetteur] pas de connexion\n");
        return 1;
    }
    NkPeerId serveur;
    serveur.value = gPair.Load();

    // Sur le fil, un octet d'aiguillage (0 = complet, 1 = quantifié) précède le paquet.
    uint8 envoi[65];
    envoi[0] = 0;
    std::memcpy(envoi + 1, complet, tc);
    (void)reseau.SendTo(serveur, envoi, tc + 1);
    envoi[0] = 1;
    std::memcpy(envoi + 1, quantifie, tq);
    (void)reseau.SendTo(serveur, envoi, tq + 1);
    NkChrono::SleepMilliseconds(1000);
    reseau.Shutdown();
    return 0;
}

int main(int argc, char **argv) {
    // Sortie non tamponnée : les lignes restent lisibles même si le programme est arrêté de force.
    std::setvbuf(stdout, nullptr, _IONBF, 0);
    NkSocket::PlatformInit();
    if (argc > 1 && std::strcmp(argv[1], "recepteur") == 0) {
        return Recepteur();
    }
    return Emetteur(argc > 2 ? argv[2] : "127.0.0.1");
}
