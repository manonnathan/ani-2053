// Chapitre 11 — Exercice 1 : la liste d'avant.
// Classe chaque ligne de la première liste des manques : encore vraie, devenue fausse, ou oubliée.
#include <iostream>
#include <set>
#include <string>
#include <vector>

namespace {
    // Lit « MOT n » puis n clés.
    std::vector<std::string> LireListe() {
        std::string mot;
        long n = 0;
        std::vector<std::string> cles;
        if (!(std::cin >> mot >> n)) {
            return cles;
        }
        for (long i = 0; i < n; ++i) {
            std::string c;
            std::cin >> c;
            cles.push_back(c);
        }
        return cles;
    }
}

int main() {
    const std::vector<std::string> avant = LireListe();
    const std::vector<std::string> corrige = LireListe();
    const std::vector<std::string> apres = LireListe();
    const std::set<std::string> estCorrige(corrige.begin(), corrige.end());
    const std::set<std::string> estApres(apres.begin(), apres.end());
    const std::set<std::string> estAvant(avant.begin(), avant.end());

    long v = 0;
    long f = 0;
    long o = 0;
    for (size_t k = 0; k < avant.size(); ++k) {
        const std::string &c = avant[k];
        const char *categorie = nullptr;
        if (estCorrige.count(c) != 0) {  // la première règle passe avant
            categorie = "DEVENUE_FAUSSE";
            ++f;
        } else if (estApres.count(c) != 0) {
            categorie = "ENCORE_VRAIE";
            ++v;
        } else {
            categorie = "OUBLIEE";
            ++o;
        }
        std::cout << (k + 1) << ' ' << c << ' ' << categorie << '\n';
    }
    long nouvelles = 0;
    for (const std::string &c : apres) {
        nouvelles += estAvant.count(c) == 0 ? 1 : 0;
    }
    std::cout << "VRAIES " << v << '\n';
    std::cout << "FAUSSES " << f << '\n';
    std::cout << "OUBLIEES " << o << '\n';
    std::cout << "NOUVELLES " << nouvelles << '\n';
    std::cout << "PART_OUBLIEE " << ((v + o) == 0 ? 0 : (100 * o) / (v + o)) << '\n';
    return 0;
}
