// Chapitre 11 — Exercice 6 : cinquante, puis deux cents.
// Ce que coûtent l'écriture et la relecture d'une planche de N images, avec le vrai format de NkRef.
//   mesure.exe 50     puis     mesure.exe 200
// Planche : N images de 100 x 80, sans trait de crayon, vue par défaut ; chaque image porte les
// mêmes 1000 octets source. Chaque durée est la moyenne de 20 appels.
#include <cstdio>
#include <cstdlib>

// Ordre imposé : NkRefFile.h (qui tire NKMath) AVANT NkChrono.h. Dans l'autre ordre, le kit du
// 22 octobre ne compile pas : NKMath/NkRandom.h:320 appelle time(nullptr) sans qualification, et
// trouve l'espace de noms nkentseu::time déclaré par NKTime (« unexpected namespace name 'time' »).
#include "NkRef/NkRefFile.h"
#include "NKTime/NkChrono.h"

using nkentseu::NkChrono;
using nkentseu::NkString;
using nkentseu::NkVector;
using nkentseu::uint8;
using nkentseu::usize;
using namespace nkref;

int main(int argc, char **argv) {
    const int n = (argc > 1) ? std::atoi(argv[1]) : 50;
    if (n <= 0) {
        std::printf("usage : mesure N (nombre d'images, au moins 1)\n");
        return 1;
    }
    constexpr int kAppels = 20;

    // Les mêmes 1000 octets source pour toutes les images, inventés.
    static uint8 source[1000];
    for (int i = 0; i < 1000; ++i) {
        source[i] = static_cast<uint8>((i * 37 + 11) & 0xFF);
    }

    // La planche : N images de 100 x 80 en grille, aucun trait.
    NkRefBoard planche;
    NkVector<NkRefFileImage> images;
    for (int i = 0; i < n; ++i) {
        planche.AddItem({static_cast<float>((i % 10) * 120), static_cast<float>((i / 10) * 100)}, 100, 80, NkString(""));
        images.PushBack({source, sizeof(source)});
    }
    const NkRefView vue;  // valeur par défaut

    // Écrire : 20 appels.
    NkVector<uint8> octets;
    NkChrono chronoEcrire;
    for (int k = 0; k < kAppels; ++k) {
        NkRefSerialize(octets, planche, images.Data(), vue, false);
    }
    const double ecrireMs = chronoEcrire.Elapsed().milliseconds / kAppels;

    // Relire : 20 appels, sur les octets écrits.
    NkVector<NkRefLoadedItem> items;
    NkVector<NkRefStroke> traits;
    NkRefView vueLue;
    bool sombre = false;
    bool grille = true;
    bool ok = true;
    NkChrono chronoRelire;
    for (int k = 0; k < kAppels; ++k) {
        ok = NkRefDeserialize(octets.Data(), octets.Size(), items, traits, vueLue, sombre, grille) && ok;
    }
    const double relireMs = chronoRelire.Elapsed().milliseconds / kAppels;

    std::printf("images : %d, octets : %u, ecrire : %.3f ms, relire : %.3f ms%s\n", n,
                static_cast<unsigned>(octets.Size()), ecrireMs, relireMs,
                (ok && items.Size() == static_cast<usize>(n)) ? "" : " (RELECTURE ECHOUEE)");
    return ok ? 0 : 1;
}
