// Chapitre 6 — Exercice 4 : l'étreinte fatale.
// Deux fils, deux verrous. La version qui se fige est gardée en commentaire ;
// la version corrigée garde les deux verrous ET les deux attentes.
//
// ---------------------------------------------------------------------------
// VERSION QUI SE FIGE (à chaque lancement, grâce aux 100 ms d'attente) :
//
//   NkThread a([](void *) {
//       NkScopedLock<NkMutex> v1(m1);           // a tient m1
//       NkChrono::SleepMilliseconds(100);
//       NkScopedLock<NkMutex> v2(m2);           // a attend m2, tenu par b
//       std::printf("a : les deux verrous\n");
//   });
//   NkThread b([](void *) {
//       NkScopedLock<NkMutex> v1(m2);           // b tient m2
//       NkChrono::SleepMilliseconds(100);
//       NkScopedLock<NkMutex> v2(m1);           // b attend m1, tenu par a
//       std::printf("b : les deux verrous\n");
//   });
//   a.Join();                                   // le fil principal attend a, pour toujours
//   b.Join();
// ---------------------------------------------------------------------------
//
// CORRECTION : un ordre unique de prise des verrous. Tout le programme prend
// m1 avant m2 ; b ne peut plus tenir m2 en attendant m1.
#include <cstdio>
// NkFunction (tiré par NkThread) fait un placement new sans inclure <new> :
// le moteur l'obtient par son en-tête précompilé, un programme seul doit l'inclure.
#include <new>

#include "NKThreading/NkMutex.h"
#include "NKThreading/NkScopedLock.h"
#include "NKThreading/NkThread.h"
#include "NKTime/NkChrono.h"

using nkentseu::NkChrono;
using nkentseu::threading::NkMutex;
using nkentseu::threading::NkScopedLock;
using nkentseu::threading::NkThread;

namespace {
    NkMutex m1;
    NkMutex m2;
}

int main() {
    NkThread a([](void *) {
        NkScopedLock<NkMutex> v1(m1);
        NkChrono::SleepMilliseconds(100);
        NkScopedLock<NkMutex> v2(m2);
        std::printf("a : les deux verrous\n");
    });
    NkThread b([](void *) {
        // Même ordre que a : m1, puis m2.
        NkScopedLock<NkMutex> v1(m1);
        NkChrono::SleepMilliseconds(100);
        NkScopedLock<NkMutex> v2(m2);
        std::printf("b : les deux verrous\n");
    });
    a.Join();
    b.Join();
    std::printf("fin : aucun blocage\n");
    return 0;
}
