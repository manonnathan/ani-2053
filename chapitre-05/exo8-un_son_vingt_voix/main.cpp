// Chapitre 5 — Exercice 8 : un son, vingt voix.
// Trois programmes sur la même rafale : une voix par lecture, rechargement, voix unique.
#include <iostream>
#include <vector>

int main() {
    long long d = 0;
    long long c = 0;
    long long n = 0;
    std::cin >> d >> c >> n;
    std::vector<long long> t(static_cast<size_t>(n > 0 ? n : 0));
    for (long long i = 0; i < n; ++i) {
        std::cin >> t[static_cast<size_t>(i)];
    }

    long long voixMax = 0;
    long long retardMax = 0;
    long long coupes = 0;
    long long finChargement = 0;

    for (long long i = 0; i < n; ++i) {
        long long ti = t[static_cast<size_t>(i)];

        // Voix actives : lectures déjà lancées, celle-ci comprise, fin exclue.
        long long voix = 0;
        for (long long j = 0; j <= i; ++j) {
            if (t[static_cast<size_t>(j)] + d > ti) {
                ++voix;
            }
        }

        // Chargements en série : chacun attend la fin du précédent.
        long long debut = ti;
        if (i > 0 && finChargement > debut) {
            debut = finChargement;
        }
        finChargement = debut + c;
        long long retard = finChargement - ti;

        // Voix unique : coupée si la suivante commence avant sa fin.
        const char *etat = "ENTIER";
        if (i + 1 < n && t[static_cast<size_t>(i + 1)] < ti + d) {
            etat = "COUPE";
            ++coupes;
        }

        if (voix > voixMax) {
            voixMax = voix;
        }
        if (retard > retardMax) {
            retardMax = retard;
        }
        std::cout << ti << ' ' << voix << ' ' << retard << ' ' << etat << '\n';
    }

    std::cout << "VOIX_MAX " << voixMax << '\n';
    std::cout << "RETARD_MAX " << retardMax << '\n';
    std::cout << "COUPES " << coupes << '\n';
    return 0;
}
