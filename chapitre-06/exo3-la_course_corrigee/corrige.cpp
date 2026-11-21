// Chapitre 6 — Exercice 3 : la course, corrigée.
// Le même compteur, corrigé de trois façons, et la version fautive mesurée aussi.
// Chaque version est mesurée trois fois ; on retient le temps médian.
// Le verrou est pris autour de CHAQUE incrément (pas autour de toute la boucle).
#include <cstdio>
// NkFunction (tiré par NkThread) fait un placement new sans inclure <new> :
// le moteur l'obtient par son en-tête précompilé, un programme seul doit l'inclure.
#include <new>

#include "NKCore/NkAtomic.h"
#include "NKThreading/NkMutex.h"
#include "NKThreading/NkScopedLock.h"
#include "NKThreading/NkThread.h"
#include "NKTime/NkChrono.h"

using nkentseu::NkAtomicInt;
using nkentseu::NkChrono;
using nkentseu::threading::NkMutex;
using nkentseu::threading::NkScopedLock;
using nkentseu::threading::NkThread;

namespace {
    constexpr int kFils = 8;
    constexpr int kIncrements = 1000000;

    long long gSansProtection = 0;

    long long gAvecMutex = 0;
    NkMutex gMutex;

    // NkAtomicInt est sur 32 bits : 8 000 000 y tient largement.
    NkAtomicInt gAtomique(0);

    long long gLocaux[kFils];

    template <typename Corps>
    double Mesure(Corps corps) {
        static NkThread fils[kFils];
        NkChrono chrono;
        for (int f = 0; f < kFils; ++f) {
            fils[f] = NkThread([corps, f](void *) {
                corps(f);
            });
        }
        for (int f = 0; f < kFils; ++f) {
            fils[f].Join();
        }
        return chrono.Elapsed().milliseconds;
    }

    double Mediane(double a, double b, double c) {
        if ((a <= b && b <= c) || (c <= b && b <= a)) {
            return b;
        }
        if ((b <= a && a <= c) || (c <= a && a <= b)) {
            return a;
        }
        return c;
    }
}

int main() {
    const long long attendu = static_cast<long long>(kFils) * kIncrements;
    std::printf("total attendu : %lld\n", attendu);

    double t[3];
    long long total = 0;

    // 1. Sans protection : faux, et différent à chaque fois.
    for (int e = 0; e < 3; ++e) {
        gSansProtection = 0;
        t[e] = Mesure([](int) {
            for (int i = 0; i < kIncrements; ++i) {
                gSansProtection += 1;
            }
        });
        total = gSansProtection;
    }
    std::printf("Sans protection : %lld %.3f ms (%.3f %.3f %.3f)\n", total, Mediane(t[0], t[1], t[2]), t[0], t[1], t[2]);

    // 2. Mutex : chaque incrément prend et rend le verrou, par un verrou de portée.
    for (int e = 0; e < 3; ++e) {
        gAvecMutex = 0;
        t[e] = Mesure([](int) {
            for (int i = 0; i < kIncrements; ++i) {
                NkScopedLock<NkMutex> verrou(gMutex);
                gAvecMutex += 1;
            }
        });
        total = gAvecMutex;
    }
    std::printf("Mutex : %lld %.3f ms (%.3f %.3f %.3f)\n", total, Mediane(t[0], t[1], t[2]), t[0], t[1], t[2]);

    // 3. Atomique : une instruction indivisible par incrément.
    for (int e = 0; e < 3; ++e) {
        gAtomique.Store(0);
        t[e] = Mesure([](int) {
            for (int i = 0; i < kIncrements; ++i) {
                gAtomique.FetchAdd(1);
            }
        });
        total = gAtomique.Load();
    }
    std::printf("Atomique : %lld %.3f ms (%.3f %.3f %.3f)\n", total, Mediane(t[0], t[1], t[2]), t[0], t[1], t[2]);

    // 4. Local : chaque fil compte chez lui ; on additionne après les Join.
    for (int e = 0; e < 3; ++e) {
        t[e] = Mesure([](int f) {
            long long local = 0;
            for (int i = 0; i < kIncrements; ++i) {
                local += 1;
            }
            gLocaux[f] = local;
        });
        total = 0;
        for (int f = 0; f < kFils; ++f) {
            total += gLocaux[f];
        }
    }
    std::printf("Local : %lld %.3f ms (%.3f %.3f %.3f)\n", total, Mediane(t[0], t[1], t[2]), t[0], t[1], t[2]);
    return 0;
}
