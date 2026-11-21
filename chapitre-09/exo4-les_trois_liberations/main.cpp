// Chapitre 9 — Exercice 4 : les trois libérations.
// Rejoue un tableau de textures libérées à la main en trois endroits : fuite ou plantage.
#include <iostream>
#include <string>
#include <vector>

namespace {
    std::vector<bool> gLiberee;  // par numéro de texture (1, 2, 3...)

    // Libère « fois » fois la texture t. Rend false si le programme meurt.
    bool Liberer(int t, int fois) {
        for (int f = 0; f < fois; ++f) {
            if (gLiberee[t]) {
                return false;
            }
            gLiberee[t] = true;
        }
        return true;
    }
}

int main() {
    int s = 0;
    int v = 0;
    int fin = 0;
    int n = 0;
    std::cin >> s >> v >> fin >> n;
    gLiberee.push_back(false);  // case 0 inutilisée
    std::vector<int> tableau;   // les numéros, rangés par index
    for (int k = 1; k <= n; ++k) {
        std::string op;
        std::cin >> op;
        if (op == "AJOUTE") {
            gLiberee.push_back(false);
            tableau.push_back(static_cast<int>(gLiberee.size()) - 1);
        } else if (op == "SUPPRIME") {
            size_t i = 0;
            std::cin >> i;
            int t = tableau[i];
            if (!Liberer(t, s)) {
                std::cout << "PLANTAGE " << k << " TEXTURE " << t << '\n';
                return 0;
            }
            tableau.erase(tableau.begin() + static_cast<long>(i));  // les suivantes reculent
        } else if (op == "VIDE") {
            for (int t : tableau) {
                if (!Liberer(t, v)) {
                    std::cout << "PLANTAGE " << k << " TEXTURE " << t << '\n';
                    return 0;
                }
            }
            tableau.clear();
        }
    }
    for (int t : tableau) {
        if (!Liberer(t, fin)) {
            std::cout << "PLANTAGE FIN TEXTURE " << t << '\n';
            return 0;
        }
    }
    int creees = static_cast<int>(gLiberee.size()) - 1;
    std::vector<int> perdues;
    for (int t = 1; t <= creees; ++t) {
        if (!gLiberee[t]) {
            perdues.push_back(t);
        }
    }
    std::cout << "CREEES " << creees << '\n';
    std::cout << "FUITES " << perdues.size() << '\n';
    std::cout << "PERDUES";
    if (perdues.empty()) {
        std::cout << " AUCUN";
    }
    for (int t : perdues) {
        std::cout << ' ' << t;
    }
    std::cout << '\n';
    return 0;
}
