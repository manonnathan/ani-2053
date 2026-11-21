// Chapitre 9 — Exercice 8 : le poids d'une inclusion.
// Combien de .cpp recompiler quand un fichier change, et quel en-tête coûte le plus cher.
#include <iostream>
#include <map>
#include <string>
#include <vector>

namespace {
    std::vector<std::string> gNom;
    std::vector<std::vector<int>> gInclusPar;  // inverse des inclusions

    bool FinitPar(const std::string &s, const std::string &p) {
        return s.size() >= p.size() && s.compare(s.size() - p.size(), p.size(), p) == 0;
    }

    // Tous les fichiers atteints en remontant les inclusions, f compris ; on compte les .cpp.
    int ARecompiler(int f) {
        std::vector<bool> vu(gNom.size(), false);
        std::vector<int> pile{f};
        vu[f] = true;
        int r = 0;
        while (!pile.empty()) {
            int x = pile.back();
            pile.pop_back();
            if (FinitPar(gNom[x], ".cpp")) {
                ++r;  // compté une seule fois, même atteint par deux chemins
            }
            for (int y : gInclusPar[x]) {
                if (!vu[y]) {
                    vu[y] = true;
                    pile.push_back(y);
                }
            }
        }
        return r;
    }
}

int main() {
    int n = 0;
    std::cin >> n;
    std::map<std::string, int> indice;
    std::vector<std::vector<std::string>> inclut(n > 0 ? n : 0);
    for (int i = 0; i < n; ++i) {
        std::string nom;
        int k = 0;
        std::cin >> nom >> k;
        gNom.push_back(nom);
        indice[nom] = i;
        for (int j = 0; j < k; ++j) {
            std::string inc;
            std::cin >> inc;
            inclut[i].push_back(inc);
        }
    }
    gInclusPar.assign(gNom.size(), {});
    for (int i = 0; i < n; ++i) {
        for (const std::string &inc : inclut[i]) {
            auto it = indice.find(inc);
            if (it != indice.end()) {  // cstdio et compagnie : ignorés
                gInclusPar[it->second].push_back(i);
            }
        }
    }
    int q = 0;
    std::cin >> q;
    for (int i = 0; i < q; ++i) {
        std::string nom;
        std::cin >> nom;
        std::cout << nom << ' ' << ARecompiler(indice[nom]) << '\n';
    }
    int meilleur = -1;
    int poids = -1;
    for (int i = 0; i < n; ++i) {
        if (FinitPar(gNom[i], ".h")) {
            int r = ARecompiler(i);
            if (r > poids) {  // strict : le premier lu garde l'égalité
                poids = r;
                meilleur = i;
            }
        }
    }
    if (meilleur < 0) {
        std::cout << "PLUS_LOURD AUCUN\n";
    } else {
        std::cout << "PLUS_LOURD " << gNom[meilleur] << ' ' << poids << '\n';
    }
    return 0;
}
