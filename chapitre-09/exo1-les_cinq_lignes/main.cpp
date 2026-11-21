// Chapitre 9 — Exercice 1 : les cinq lignes.
// Lit le corps d'une classe et dit quelle règle elle suit : zéro, trois, cinq, ou aucune.
#include <iostream>
#include <string>

namespace {
    enum Etat { ABSENT, ECRIT, DEFAUT, SUPPRIME };
    const char *const kNomEtat[] = {"ABSENT", "ECRIT", "DEFAUT", "SUPPRIME"};
    const char *const kMembre[] = {"DESTRUCTEUR", "COPIE", "AFFECTATION_COPIE", "DEPLACEMENT",
                                   "AFFECTATION_DEPLACEMENT"};

    bool CommencePar(const std::string &s, const std::string &p) {
        return s.compare(0, p.size(), p) == 0;
    }
    bool FinitPar(const std::string &s, const std::string &p) {
        return s.size() >= p.size() && s.compare(s.size() - p.size(), p.size(), p) == 0;
    }
    // Commence par p, sans un second & juste après : c'est une copie, pas un C(const C&&).
    bool CommenceParSansDoubleEt(const std::string &s, const std::string &p) {
        return CommencePar(s, p) && !(s.size() > p.size() && s[p.size()] == '&');
    }
}

int main() {
    std::string classe;
    if (!std::getline(std::cin, classe)) {
        classe.clear();
    }
    std::string nettoye;
    for (char c : classe) {
        if (c != ' ' && c != '\t' && c != '\r') {
            nettoye += c;
        }
    }
    classe = nettoye;

    Etat etat[5] = {ABSENT, ABSENT, ABSENT, ABSENT, ABSENT};
    std::string ligne;
    while (std::getline(std::cin, ligne)) {
        // 1. Le commentaire part : tout ce qui suit le premier //.
        std::string::size_type com = ligne.find("//");
        if (com != std::string::npos) {
            ligne.erase(com);
        }
        // 2. Toutes les espaces partent.
        std::string l;
        for (char c : ligne) {
            if (c != ' ' && c != '\t' && c != '\r') {
                l += c;
            }
        }
        if (l.empty()) {
            continue;
        }
        // 3. Quel membre ?
        int m = -1;
        if (l.find("~" + classe + "(") != std::string::npos) {
            m = 0;
        } else if (CommenceParSansDoubleEt(l, classe + "(const" + classe + "&")) {
            m = 1;
        } else if (CommenceParSansDoubleEt(l, classe + "&operator=(const" + classe + "&")) {
            m = 2;
        } else if (CommencePar(l, classe + "(" + classe + "&&")) {
            m = 3;
        } else if (CommencePar(l, classe + "&operator=(" + classe + "&&")) {
            m = 4;
        }
        if (m < 0) {
            continue;
        }
        if (FinitPar(l, "=delete;")) {
            etat[m] = SUPPRIME;
        } else if (FinitPar(l, "=default;")) {
            etat[m] = DEFAUT;
        } else {
            etat[m] = ECRIT;
        }
    }

    for (int i = 0; i < 5; ++i) {
        std::cout << kMembre[i] << ' ' << kNomEtat[etat[i]] << '\n';
    }
    int declares = 0;
    for (Etat e : etat) {
        declares += (e != ABSENT) ? 1 : 0;
    }
    bool unDeplacement = etat[3] != ABSENT || etat[4] != ABSENT;
    if (declares == 0) {
        std::cout << "REGLE ZERO\n";
    } else if (declares == 5) {
        std::cout << "REGLE CINQ\n";
    } else if (etat[0] != ABSENT && etat[1] != ABSENT && etat[2] != ABSENT && !unDeplacement) {
        std::cout << "REGLE TROIS\n";
    } else {
        std::cout << "REGLE INCOMPLETE\n";
        int jusque = unDeplacement ? 5 : 3;
        for (int i = 0; i < jusque; ++i) {
            if (etat[i] == ABSENT) {
                std::cout << "MANQUE " << kMembre[i] << '\n';
            }
        }
    }
    std::cout << "POSSEDE " << (etat[0] == ECRIT ? "OUI" : "NON") << '\n';
    bool unique = etat[1] == SUPPRIME && (etat[3] == ECRIT || etat[3] == DEFAUT);
    std::cout << "UNIQUE " << (unique ? "OUI" : "NON") << '\n';
    return 0;
}
