// Chapitre 10 — Exercice 9 : l'identifiant plutôt que l'alignement.
// Chiffre les deux conceptions : gestes à répéter (index), recherches (identifiant).
#include <iostream>
#include <string>
#include <utility>
#include <vector>

int main() {
    int n = 0;
    std::cin >> n;
    std::vector<int> planche;
    int prochain = 1;  // jamais réutilisé
    long long occasions = 0;
    long long repercutes = 0;
    long long recherches = 0;
    for (int k = 0; k < n; ++k) {
        std::string op;
        std::cin >> op;
        if (op == "AJOUTE") {
            planche.push_back(prochain++);  // à la fin : l'ordre ne change pas
        } else if (op == "SUPPRIME") {
            int c = 0;
            std::cin >> c;
            std::vector<long> index(c > 0 ? c : 0);
            for (long &i : index) {
                std::cin >> i;
            }
            for (auto it = index.rbegin(); it != index.rend(); ++it) {
                planche.erase(planche.begin() + *it);
            }
            ++occasions;
            repercutes += 2LL * c;  // textures et sources
        } else if (op == "MONTE" || op == "DESCEND") {
            long i = 0;
            std::cin >> i;
            long j = (op == "MONTE") ? i + 1 : i - 1;
            if (j < 0 || j >= static_cast<long>(planche.size())) {
                continue;  // rien n'a bougé : rien à compter
            }
            std::swap(planche[i], planche[j]);
            ++occasions;
            repercutes += 2;
        } else if (op == "DESSINE") {
            recherches += static_cast<long long>(planche.size());
        }
    }
    std::cout << "ORDRE";
    if (planche.empty()) {
        std::cout << " AUCUN";
    }
    for (int id : planche) {
        std::cout << ' ' << id;
    }
    std::cout << '\n';
    std::cout << "OCCASIONS " << occasions << '\n';
    std::cout << "REPERCUTES " << repercutes << '\n';
    std::cout << "RECHERCHES " << recherches << '\n';
    return 0;
}
