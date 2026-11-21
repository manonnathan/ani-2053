// Chapitre 6 — Exercice 1 : la réserve.
// Deux cents tâches courtes : soumises à une NkThreadPool, puis confiées à deux cents
// NkThread créés à la main. Même tâche, même mesure, trois fois chacune.
//
// Construire en Release : jenga build --target Reserve --config Release
#include <cstdio>
// NkFunction (tiré par NkThread) fait un placement new sans inclure <new> :
// le moteur l'obtient par son en-tête précompilé, un programme seul doit l'inclure.
#include <new>

#include "NKThreading/NkThread.h"
#include "NKThreading/NkThreadPool.h"
#include "NKTime/NkChrono.h"

using nkentseu::NkChrono;
using nkentseu::threading::NkThread;
using nkentseu::threading::NkThreadPool;

namespace {
    constexpr int kNbTaches = 200;

    // Multiplie la taille de la tâche : 1 pour la tâche courte, 100 pour la longue.
    int gFacteur = 1;

    // Les entiers à additionner. Ils sont lus dans un tableau rempli au démarrage :
    // une boucle « somme += i » serait remplacée en Release par une formule, et la
    // tâche « cent fois plus longue » ne durerait pas plus longtemps.
    constexpr int kMaxEntiers = 100000;
    int gSource[kMaxEntiers];

    // Chaque tâche écrit son résultat dans sa propre case : rien n'est partagé,
    // et le compilateur ne peut pas retirer un calcul dont le résultat sert.
    long long gResultats[kNbTaches];

    // La tâche : additionner mille entiers (fois le facteur) dans une variable locale.
    void Tache(int indice) {
        long long somme = 0;
        const int n = 1000 * gFacteur;
        for (int i = 0; i < n; ++i) {
            somme += gSource[i];
        }
        gResultats[indice] = somme;
    }

    long long Controle() {
        long long total = 0;
        for (int i = 0; i < kNbTaches; ++i) {
            total += gResultats[i];
        }
        return total;
    }

    double VersionReserve(NkThreadPool &pool) {
        NkChrono chrono;
        for (int i = 0; i < kNbTaches; ++i) {
            pool.Enqueue([i]() {
                Tache(i);
            });
        }
        pool.Join();
        return chrono.Elapsed().milliseconds;
    }

    double VersionFils() {
        static NkThread fils[kNbTaches];
        NkChrono chrono;
        for (int i = 0; i < kNbTaches; ++i) {
            fils[i] = NkThread([i](void *) {
                Tache(i);
            });
        }
        // Chaque fil créé est joint : aucun ne survit à la mesure.
        for (int i = 0; i < kNbTaches; ++i) {
            fils[i].Join();
        }
        return chrono.Elapsed().milliseconds;
    }
}

int main() {
    for (int i = 0; i < kMaxEntiers; ++i) {
        gSource[i] = (i * 7) ^ (i >> 3);
    }
    NkThreadPool pool;
    std::printf("Ouvriers : %u\n", pool.GetNumWorkers());

    for (int passe = 0; passe < 2; ++passe) {
        gFacteur = (passe == 0) ? 1 : 100;
        std::printf("--- Tache %s (%d entiers) ---\n", passe == 0 ? "courte" : "longue", 1000 * gFacteur);
        // La première mesure paie des chargements : on en garde trois.
        for (int essai = 1; essai <= 3; ++essai) {
            double reserve = VersionReserve(pool);
            long long ctrlReserve = Controle();
            double fils = VersionFils();
            long long ctrlFils = Controle();
            std::printf("essai %d : reserve %.3f ms | fils %.3f ms | controle %lld %lld\n", essai, reserve, fils,
                        ctrlReserve, ctrlFils);
        }
    }
    return 0;
}
