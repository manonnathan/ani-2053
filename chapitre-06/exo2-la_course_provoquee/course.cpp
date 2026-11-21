// Chapitre 6 — Exercice 2 : la course, provoquée.
// Huit fils incrémentent un même compteur, un million de fois chacun, sans protection.
// Usage : course [nombre_de_fils]   (8 par défaut, 1 pour la contre-épreuve)
#include <cstdio>
// NkFunction (tiré par NkThread) fait un placement new sans inclure <new> :
// le moteur l'obtient par son en-tête précompilé, un programme seul doit l'inclure.
#include <new>
#include <cstdlib>

#include "NKThreading/NkThread.h"

using nkentseu::threading::NkThread;

namespace {
    constexpr int kIncrements = 1000000;
    constexpr int kMaxFils = 64;

    // Partagé, sans rien pour ordonner les accès : c'est la course.
    long long compteur = 0;
}

int main(int argc, char **argv) {
    int nbFils = 8;
    if (argc > 1) {
        nbFils = std::atoi(argv[1]);
    }
    if (nbFils < 1 || nbFils > kMaxFils) {
        nbFils = 8;
    }

    static NkThread fils[kMaxFils];
    for (int f = 0; f < nbFils; ++f) {
        fils[f] = NkThread([](void *) {
            for (int i = 0; i < kIncrements; ++i) {
                compteur += 1;
            }
        });
    }
    // Le total se lit APRÈS les Join, jamais avant.
    for (int f = 0; f < nbFils; ++f) {
        fils[f].Join();
    }

    long long attendu = static_cast<long long>(nbFils) * kIncrements;
    std::printf("fils : %d\n", nbFils);
    std::printf("total obtenu : %lld\n", compteur);
    std::printf("total attendu : %lld\n", attendu);
    std::printf("perdus : %lld\n", attendu - compteur);
    return 0;
}
