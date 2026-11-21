// Chapitre 9 — Exercice 6 : le pointeur qui possède.
// Rejoue des pointeurs uniques : chaque objet meurt une fois, et la copie ne compile pas.
#include <iostream>
#include <map>
#include <string>
#include <vector>

namespace {
    std::map<std::string, int> gTient;  // 0 : nul
    std::vector<std::string> gApparition;
    int gDetruits = 0;

    int &Var(const std::string &nom) {
        if (gTient.find(nom) == gTient.end()) {
            gTient[nom] = 0;  // une variable jamais vue existe déjà, nulle
            gApparition.push_back(nom);
        }
        return gTient[nom];
    }

    void Detruire(int &o) {
        if (o != 0) {
            std::cout << "DETRUIT " << o << '\n';
            ++gDetruits;
            o = 0;
        }
    }
}

int main() {
    int n = 0;
    std::cin >> n;
    std::vector<std::string> op(n > 0 ? n : 0);
    std::vector<std::string> a(op.size());
    std::vector<std::string> b(op.size());
    bool copie = false;
    for (int k = 0; k < n; ++k) {
        std::cin >> op[k] >> a[k];
        if (op[k] == "MOVE" || op[k] == "COPY") {
            std::cin >> b[k];
        }
        copie = copie || op[k] == "COPY";
    }
    // Une seule copie, et rien ne compile : aucun objet n'est créé.
    if (copie) {
        for (int k = 0; k < n; ++k) {
            if (op[k] == "COPY") {
                std::cout << "REFUSE " << (k + 1) << '\n';
            }
        }
        return 0;
    }
    int crees = 0;
    int nuls = 0;
    for (int k = 0; k < n; ++k) {
        if (op[k] == "NEW") {
            int &v = Var(a[k]);
            int nouveau = ++crees;
            Detruire(v);  // l'ancien objet meurt après la création du nouveau
            v = nouveau;
        } else if (op[k] == "MOVE") {
            int &src = Var(a[k]);
            int &dst = Var(b[k]);
            int pris = src;
            src = 0;
            Detruire(dst);
            dst = pris;  // même nul : la destination est vidée
        } else if (op[k] == "RESET") {
            Detruire(Var(a[k]));
        } else if (op[k] == "USE") {
            if (Var(a[k]) == 0) {
                std::cout << "NUL " << (k + 1) << ' ' << a[k] << '\n';
                ++nuls;
            }
        }
    }
    // Fin de portée : ordre inverse de la première apparition.
    for (auto it = gApparition.rbegin(); it != gApparition.rend(); ++it) {
        Detruire(gTient[*it]);
    }
    std::cout << "CREES " << crees << '\n';
    std::cout << "DETRUITS " << gDetruits << '\n';
    std::cout << "NULS " << nuls << '\n';
    return 0;
}
