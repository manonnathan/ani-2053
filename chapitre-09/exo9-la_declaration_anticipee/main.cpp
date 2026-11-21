// Chapitre 9 — Exercice 9 : la déclaration anticipée.
// Pour chaque type : inclure sa définition, ou une déclaration anticipée suffit ?
#include <iostream>
#include <map>
#include <set>
#include <string>
#include <vector>

int main() {
    int u = 0;
    std::cin >> u;
    std::vector<std::string> ordre;          // première apparition
    std::map<std::string, bool> aInclure;
    for (int i = 0; i < u; ++i) {
        std::string type;
        std::string emploi;
        std::cin >> type >> emploi;
        if (aInclure.find(type) == aInclure.end()) {
            aInclure[type] = false;
            ordre.push_back(type);
        }
        // Taille ou membres nécessaires : la définition complète.
        if (emploi == "VALEUR" || emploi == "BASE" || emploi == "CORPS" || emploi == "TAILLE") {
            aInclure[type] = true;
        }
    }
    int a = 0;
    std::cin >> a;
    std::set<std::string> retirees;
    for (int i = 0; i < a; ++i) {
        std::string type;
        std::cin >> type;
        auto it = aInclure.find(type);
        if (it != aInclure.end() && !it->second) {
            retirees.insert(type);
        }
    }
    int inclure = 0;
    for (const std::string &t : ordre) {
        std::cout << t << (aInclure[t] ? " INCLURE" : " ANTICIPER") << '\n';
        inclure += aInclure[t] ? 1 : 0;
    }
    std::cout << "INCLURE " << inclure << '\n';
    std::cout << "ANTICIPER " << (static_cast<int>(ordre.size()) - inclure) << '\n';
    std::cout << "INCLUSIONS_RETIREES " << retirees.size() << '\n';
    return 0;
}
