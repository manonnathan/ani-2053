// Chapitre 4 — Exercice 5 : le cercle qui n'en est pas un.
// Calcule à partir de quel agrandissement les segments d'un cercle se voient.
#include <cmath>
#include <iostream>

int main() {
    const double pi = 3.141592653589793;
    long long n = 0;
    if (!(std::cin >> n)) {
        n = 0;
    }
    long long visibles = 0;
    long long refuses = 0;

    for (long long i = 0; i < n; ++i) {
        long long r = 0;
        long long segments = 0;
        std::cin >> r >> segments;

        if (segments < 3) {
            std::cout << r << ' ' << segments << " REFUSE\n";
            ++refuses;
            continue;
        }
        // Écart maximal entre l'arc et la corde, au milieu du segment.
        double g = static_cast<double>(r) * (1.0 - std::cos(pi / static_cast<double>(segments)));
        long long ecart = static_cast<long long>(std::floor(g * 1000.0));

        if (g == 0.0) {
            // Rayon nul : aucun agrandissement ne montre rien, et on ne divise pas.
            std::cout << r << ' ' << segments << ' ' << ecart << " JAMAIS\n";
            continue;
        }
        long long zoom = static_cast<long long>(std::ceil(100.0 / g));
        const char *verdict = "INVISIBLE";
        if (zoom <= 100) {
            verdict = "VISIBLE";
            ++visibles;
        }
        std::cout << r << ' ' << segments << ' ' << ecart << ' ' << zoom << ' ' << verdict << '\n';
    }

    std::cout << "VISIBLES " << visibles << '\n';
    std::cout << "REFUSES " << refuses << '\n';
    return 0;
}
