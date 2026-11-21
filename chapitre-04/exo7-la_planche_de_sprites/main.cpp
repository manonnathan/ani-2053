// Chapitre 4 — Exercice 7 : la planche de sprites.
// Choisit la case d'une animation à partir du temps écoulé, plafonné.
#include <iostream>

int main() {
    long long colonnes = 0;
    long long lignes = 0;
    long long w = 0;
    long long h = 0;
    long long f = 1;
    long long d = 1;
    long long p = 1;
    std::cin >> colonnes >> lignes >> w >> h >> f >> d >> p;
    (void)lignes;

    long long n = 0;
    std::cin >> n;

    long long caseCourante = 0;
    long long accumule = 0;
    long long avances = 0;
    long long plafonnes = 0;

    for (long long i = 0; i < n; ++i) {
        long long dt = 0;
        std::cin >> dt;
        // Le plafond s'applique AVANT l'accumulation ; un dt égal au plafond ne compte pas.
        if (dt > p) {
            dt = p;
            ++plafonnes;
        }
        accumule += dt;
        // On retire D sans remettre à zéro : le reste sert à l'image suivante.
        while (accumule >= d) {
            accumule -= d;
            caseCourante = (caseCourante + 1) % f;
            ++avances;
        }
        long long x = (caseCourante % colonnes) * w;
        long long y = (caseCourante / colonnes) * h;
        std::cout << caseCourante << ' ' << x << ' ' << y << ' ' << w << ' ' << h << '\n';
    }

    std::cout << "AVANCES " << avances << '\n';
    std::cout << "PLAFONNES " << plafonnes << '\n';
    return 0;
}
