// Chapitre 9 — Exercice 3 : le const manquant.
// Trouve les méthodes qui pourraient être const, les racines, et le nombre de passes.
#include <iostream>
#include <map>
#include <string>
#include <vector>

namespace {
    struct Methode {
        std::string nom;
        bool declaree = false;
        bool modifie = false;
        std::vector<int> appelle;
    };

    std::vector<Methode> gM;
    std::vector<int> gMemo;  // -1 inconnu, 0 non, 1 oui

    // Déclarée const, ou pourrait l'être. L'entrée garantit l'absence de boucle.
    bool ConstOuPourrait(int i) {
        if (gM[i].declaree) {
            return true;
        }
        if (gMemo[i] >= 0) {
            return gMemo[i] == 1;
        }
        bool ok = !gM[i].modifie;
        for (int j : gM[i].appelle) {
            if (!ok) {
                break;
            }
            ok = ConstOuPourrait(j);
        }
        gMemo[i] = ok ? 1 : 0;
        return ok;
    }

    void Liste(const char *etiquette, const std::vector<int> &l) {
        std::cout << etiquette;
        if (l.empty()) {
            std::cout << " AUCUN";
        }
        for (int i : l) {
            std::cout << ' ' << gM[i].nom;
        }
        std::cout << '\n';
    }
}

int main() {
    int n = 0;
    std::cin >> n;
    gM.resize(n > 0 ? n : 0);
    std::vector<std::vector<std::string>> appels(gM.size());
    std::map<std::string, int> indice;
    for (int i = 0; i < n; ++i) {
        std::string declaree;
        int modifie = 0;
        int k = 0;
        std::cin >> gM[i].nom >> declaree >> modifie >> k;
        gM[i].declaree = (declaree == "const");
        gM[i].modifie = (modifie != 0);
        for (int a = 0; a < k; ++a) {
            std::string nom;
            std::cin >> nom;
            appels[i].push_back(nom);
        }
        indice[gM[i].nom] = i;
    }
    for (size_t i = 0; i < gM.size(); ++i) {
        for (const std::string &nom : appels[i]) {
            gM[i].appelle.push_back(indice[nom]);
        }
    }
    gMemo.assign(gM.size(), -1);

    std::vector<int> pourrait;
    std::vector<int> racines;
    for (size_t i = 0; i < gM.size(); ++i) {
        if (gM[i].declaree || !ConstOuPourrait(static_cast<int>(i))) {
            continue;
        }
        pourrait.push_back(static_cast<int>(i));
        bool racine = true;
        for (int j : gM[i].appelle) {
            racine = racine && gM[j].declaree;
        }
        if (racine) {
            racines.push_back(static_cast<int>(i));
        }
    }

    // Les passes : toutes en même temps, d'un étage à la fois.
    std::vector<bool> estConst(gM.size());
    for (size_t i = 0; i < gM.size(); ++i) {
        estConst[i] = gM[i].declaree;
    }
    int passes = 0;
    size_t faites = 0;
    while (faites < pourrait.size()) {
        std::vector<int> cettePasse;
        for (int i : pourrait) {
            if (estConst[i]) {
                continue;
            }
            bool pret = true;
            for (int j : gM[i].appelle) {
                pret = pret && estConst[j];
            }
            if (pret) {
                cettePasse.push_back(i);
            }
        }
        if (cettePasse.empty()) {
            break;  // impossible avec une entrée valide
        }
        for (int i : cettePasse) {
            estConst[i] = true;
        }
        faites += cettePasse.size();
        ++passes;
    }

    Liste("POURRAIT", pourrait);
    Liste("RACINES", racines);
    std::cout << "PASSES " << passes << '\n';
    return 0;
}
