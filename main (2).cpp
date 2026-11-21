// Chapitre 11 — Exercice 3 : ce qu'on peut annuler.
// L'annulation attendue (une pile de toutes les opérations) contre celle de l'outil (les traits seuls).
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

int main() {
    std::vector<std::string> pile;  // ce que l'utilisateur croit pouvoir défaire
    std::vector<long> traits;       // ce que l'outil tient : les traits présents
    long prochainTrait = 1;
    long o = 0;
    long a = 0;
    long k = 0;
    long justes = 0;
    long riens = 0;
    long trahis = 0;
    std::string ligne;
    while (std::getline(std::cin, ligne)) {
        std::istringstream in(ligne);
        std::string op;
        if (!(in >> op)) {
            continue;
        }
        if (op == "ANNULE") {
            std::string x = "AUCUNE";
            if (!pile.empty()) {
                x = pile.back();
                pile.pop_back();
            }
            std::string y = "RIEN";
            if (!traits.empty()) {
                y = "TRAIT " + std::to_string(traits.back());
                traits.pop_back();
            }
            const char *verdict = nullptr;
            if ((x == "AUCUNE" && y == "RIEN") || x == y) {
                verdict = "JUSTE";
                ++justes;
            } else if (y == "RIEN") {
                verdict = "RIEN";
                ++riens;
            } else {
                verdict = "TRAHI";
                ++trahis;
            }
            std::cout << "ANNULE " << ++k << " ATTENDU " << x << " OBTENU " << y << ' ' << verdict << '\n';
            continue;
        }
        ++o;
        if (op == "TRAIT") {
            const long n = prochainTrait++;
            traits.push_back(n);
            pile.push_back("TRAIT " + std::to_string(n));
            ++a;
        } else if (op == "EFFACE") {
            traits.clear();  // l'outil ne sait pas défaire EFFACE
            pile.push_back("EFFACE");
        } else if (op == "RANGE") {
            pile.push_back("RANGE");
        } else {
            std::string nom;
            in >> nom;
            pile.push_back(op + " " + nom);  // DEPOSE, DEPLACE, SUPPRIME, MIROIR
        }
    }
    std::cout << "JUSTES " << justes << '\n';
    std::cout << "RIENS " << riens << '\n';
    std::cout << "TRAHIS " << trahis << '\n';
    std::cout << "ANNULABLES " << a << " SUR " << o << '\n';
    return 0;
}
