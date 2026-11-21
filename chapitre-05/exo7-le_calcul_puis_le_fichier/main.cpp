// Chapitre 5 — Exercice 7 : le calcul, puis le fichier.
// Taille brute d'un son, part du fichier, et décision mémoire ou flux.
#include <iostream>
#include <string>

int main() {
    long long seuil = 0;
    long long n = 0;
    std::cin >> seuil >> n;
    long long memoire = 0;
    long long flux = 0;
    long long refuses = 0;

    for (long long i = 0; i < n; ++i) {
        std::string nom;
        long long frequence = 0;
        long long canaux = 0;
        long long bits = 0;
        long long duree = 0;
        long long fichier = 0;
        std::cin >> nom >> frequence >> canaux >> bits >> duree >> fichier;

        if (bits != 8 && bits != 16 && bits != 24 && bits != 32) {
            std::cout << nom << " REFUSE\n";
            ++refuses;
            continue;
        }
        // Multiplications d'abord, en 64 bits ; la division par 1000 à la fin.
        long long brut = frequence * canaux * (bits / 8) * duree / 1000;
        long long pourcent = 0;
        if (brut > 0) {
            pourcent = fichier * 100 / brut;
        }
        const char *mode = "MEMOIRE";
        if (brut > seuil) {
            mode = "FLUX";
            ++flux;
        } else {
            memoire += brut;
        }
        std::cout << nom << ' ' << brut << ' ' << pourcent << ' ' << mode << '\n';
    }

    std::cout << "MEMOIRE " << memoire << '\n';
    std::cout << "FLUX " << flux << '\n';
    std::cout << "REFUSES " << refuses << '\n';
    return 0;
}
