// Chapitre 9 — Exercice 5 : l'ordre qui tue.
// Rejoue la destruction des objets locaux, bloc par bloc, et les fermetures explicites.
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

namespace {
    std::vector<std::pair<std::string, std::string>> gDepend;  // dans l'ordre de lecture
    std::map<std::string, bool> gLibere;

    // Libère a. Rend false (et affiche PLANTAGE) si a a besoin d'un objet déjà libéré.
    bool Liberer(const std::string &a, const char *verbe) {
        if (gLibere[a]) {
            return true;  // déjà libéré : rien, et rien ne s'affiche
        }
        for (const auto &d : gDepend) {
            if (d.first == a && gLibere[d.second]) {
                std::cout << "PLANTAGE " << a << ' ' << d.second << '\n';
                return false;
            }
        }
        gLibere[a] = true;
        std::cout << verbe << ' ' << a << '\n';
        return true;
    }

    // Ferme un bloc : du dernier déclaré au premier.
    bool FermerBloc(const std::vector<std::string> &bloc) {
        for (auto it = bloc.rbegin(); it != bloc.rend(); ++it) {
            if (!Liberer(*it, "DETRUIT")) {
                return false;
            }
        }
        return true;
    }
}

int main() {
    std::vector<std::vector<std::string>> blocs(1);  // le bloc de la fonction
    std::string ligne;
    while (std::getline(std::cin, ligne)) {
        std::istringstream in(ligne);
        std::string mot;
        if (!(in >> mot)) {
            continue;
        }
        if (mot == "DECL") {
            std::string x;
            in >> x;
            blocs.back().push_back(x);
            gLibere[x] = false;
        } else if (mot == "DEPEND") {
            std::string a;
            std::string b;
            in >> a >> b;
            gDepend.emplace_back(a, b);
        } else if (mot == "FERME") {
            std::string x;
            in >> x;
            if (!Liberer(x, "FERME")) {
                return 0;
            }
        } else if (mot == "{") {
            blocs.emplace_back();
        } else if (mot == "}") {
            if (!FermerBloc(blocs.back())) {
                return 0;
            }
            blocs.pop_back();
        }
    }
    // La fin de l'entrée ferme le bloc de la fonction (et tout bloc resté ouvert).
    while (!blocs.empty()) {
        if (!FermerBloc(blocs.back())) {
            return 0;
        }
        blocs.pop_back();
    }
    std::cout << "PROPRE\n";
    return 0;
}
