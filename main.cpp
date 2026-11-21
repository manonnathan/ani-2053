// Chapitre 11 — Exercice 2 : le texte qui ment.
// Confronte les annonces de l'interface à l'historique : lesquelles mentent, et depuis quel commit.
#include <iostream>
#include <map>
#include <set>
#include <string>
#include <vector>

int main() {
    long h = 0;
    std::cin >> h;
    std::vector<std::string> hash;
    std::map<std::string, long> indice;
    std::vector<std::set<std::string>> etat;  // fonctions présentes APRÈS chaque commit
    std::set<std::string> present;            // avant le premier commit : rien
    for (long i = 0; i < h; ++i) {
        std::string id;
        std::string action;
        long k = 0;
        std::cin >> id >> action >> k;
        for (long j = 0; j < k; ++j) {
            std::string f;
            std::cin >> f;
            if (action == "AJOUTE") {
                present.insert(f);  // déjà présente : rien ne change
            } else {
                present.erase(f);   // absente : rien ne change
            }
        }
        indice[id] = i;
        hash.push_back(id);
        etat.push_back(present);
    }

    long a = 0;
    std::cin >> a;
    long mensonges = 0;
    for (long k = 1; k <= a; ++k) {
        std::string ecrite;
        std::string type;
        std::string f;
        std::cin >> ecrite >> type >> f;
        const long debut = indice[ecrite];
        auto vraie = [&](long i) {
            const bool la = etat[i].count(f) != 0;
            return type == "A_VENIR" ? !la : la;
        };
        if (h == 0 || vraie(h - 1)) {
            std::cout << "ANNONCE " << k << " VRAI\n";
            continue;
        }
        // On remonte tant que l'annonce est fausse, sans dépasser le commit où elle a été écrite.
        long depuis = h - 1;
        while (depuis > debut && !vraie(depuis - 1)) {
            --depuis;
        }
        std::cout << "ANNONCE " << k << " MENT " << hash[depuis] << '\n';
        ++mensonges;
    }
    std::cout << "MENSONGES " << mensonges << '\n';
    return 0;
}
