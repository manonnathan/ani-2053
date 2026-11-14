// Chapitre 10 — Exercice 6 : le troisième tableau.
// Le modèle, et deux tableaux tenus alignés sur ses index : où l'alignement casse-t-il ?
#include <algorithm>
#include <iostream>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

namespace {
    using Tableau = std::vector<std::string>;

    struct Repete {
        bool textures = false;
        bool sources = false;
    };

    Repete LireLigne() {
        Repete r;
        std::string ligne;
        std::getline(std::cin, ligne);
        std::istringstream in(ligne);
        std::string mot;
        in >> mot;  // AJOUTE, SUPPRIME ou ECHANGE
        while (in >> mot) {
            r.textures = r.textures || mot == "textures";
            r.sources = r.sources || mot == "sources";
        }
        return r;
    }

    void Retirer(Tableau &t, const std::vector<size_t> &index) {
        // Du plus grand au plus petit ; un index absent de ce tableau est sauté.
        for (auto it = index.rbegin(); it != index.rend(); ++it) {
            if (*it < t.size()) {
                t.erase(t.begin() + static_cast<long>(*it));
            }
        }
    }

    void Echanger(Tableau &t, size_t i, size_t j) {
        if (i < t.size() && j < t.size()) {
            std::swap(t[i], t[j]);
        }
    }
}

int main() {
    Repete ajout = LireLigne();
    Repete suppr = LireLigne();
    Repete echange = LireLigne();
    Tableau modele, textures, sources;
    int n = 0;
    std::cin >> n;
    for (int k = 0; k < n; ++k) {
        std::string op;
        std::cin >> op;
        if (op == "AJOUTE") {
            std::string nom;
            std::cin >> nom;
            modele.push_back(nom);
            if (ajout.textures) {
                textures.push_back(nom);
            }
            if (ajout.sources) {
                sources.push_back(nom);
            }
        } else if (op == "SUPPRIME") {
            int c = 0;
            std::cin >> c;
            std::vector<size_t> index(c > 0 ? c : 0);
            for (size_t &i : index) {
                std::cin >> i;
            }
            Retirer(modele, index);
            if (suppr.textures) {
                Retirer(textures, index);
            }
            if (suppr.sources) {
                Retirer(sources, index);
            }
        } else if (op == "MONTE" || op == "DESCEND") {
            long i = 0;
            std::cin >> i;
            long j = (op == "MONTE") ? i + 1 : i - 1;
            if (j < 0 || j >= static_cast<long>(modele.size())) {
                continue;  // pas de voisine dans le modèle : rien, nulle part
            }
            Echanger(modele, static_cast<size_t>(i), static_cast<size_t>(j));
            if (echange.textures) {
                Echanger(textures, static_cast<size_t>(i), static_cast<size_t>(j));
            }
            if (echange.sources) {
                Echanger(sources, static_cast<size_t>(i), static_cast<size_t>(j));
            }
        }
    }
    size_t l = std::max(modele.size(), std::max(textures.size(), sources.size()));
    int decales = 0;
    for (size_t i = 0; i < l; ++i) {
        const std::string m = i < modele.size() ? modele[i] : "-";
        const std::string t = i < textures.size() ? textures[i] : "-";
        const std::string s = i < sources.size() ? sources[i] : "-";
        std::cout << i << ' ' << m << ' ' << t << ' ' << s << '\n';
        bool aligne = m != "-" && t != "-" && s != "-" && m == t && t == s;
        decales += aligne ? 0 : 1;
    }
    std::cout << "DECALES " << decales << '\n';
    return 0;
}
