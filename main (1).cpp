// Chapitre 11 — Exercice 8 : regarder sans rien dire.
// Les contournements observés pendant vingt minutes, face à ce que la personne dit ensuite.
#include <algorithm>
#include <iostream>
#include <map>
#include <set>
#include <string>
#include <vector>

namespace {
    struct Contournement {
        std::string nom;
        long fois = 0;
        long premier = 0;  // plus petit instant observé, en secondes
        long entree = 0;   // rang de sa première ligne dans l'entrée
    };
}

int main() {
    const long kFinSeance = 20 * 60;  // 20:00 pile compte encore
    std::string mot;
    long n = 0;
    std::cin >> mot >> n;
    std::vector<Contournement> liste;
    std::map<std::string, size_t> indice;
    long horsSeance = 0;
    for (long i = 0; i < n; ++i) {
        std::string quand;
        std::string nom;
        std::cin >> quand >> nom;
        const long s = std::stol(quand.substr(0, quand.find(':'))) * 60 + std::stol(quand.substr(quand.find(':') + 1));
        if (s > kFinSeance) {
            ++horsSeance;
            continue;
        }
        auto it = indice.find(nom);
        if (it == indice.end()) {
            indice[nom] = liste.size();
            liste.push_back({nom, 1, s, i});
        } else {
            Contournement &c = liste[it->second];
            ++c.fois;
            c.premier = std::min(c.premier, s);
        }
    }
    long m = 0;
    std::cin >> mot >> m;
    std::vector<std::string> dit;
    std::set<std::string> estDit;
    for (long i = 0; i < m; ++i) {
        std::string nom;
        std::cin >> nom;
        dit.push_back(nom);
        estDit.insert(nom);
    }

    std::stable_sort(liste.begin(), liste.end(), [](const Contournement &a, const Contournement &b) {
        if (a.fois != b.fois) {
            return a.fois > b.fois;
        }
        if (a.premier != b.premier) {
            return a.premier < b.premier;
        }
        return a.entree < b.entree;
    });
    long nonDits = 0;
    long foisNonDits = 0;
    long foisTous = 0;
    for (const Contournement &c : liste) {
        const bool d = estDit.count(c.nom) != 0;
        std::cout << c.nom << ' ' << c.fois << (d ? " DIT" : " NON_DIT") << '\n';
        foisTous += c.fois;
        if (!d) {
            ++nonDits;
            foisNonDits += c.fois;
        }
    }
    std::cout << "NON_DITS " << nonDits << '\n';
    std::cout << "PART_NON_DITE " << (foisTous == 0 ? 0 : 100 * foisNonDits / foisTous) << '\n';
    std::cout << "DIT_SANS_VU";
    bool aucun = true;
    for (const std::string &nom : dit) {
        if (indice.find(nom) == indice.end()) {  // jamais observé pendant la séance
            std::cout << ' ' << nom;
            aucun = false;
        }
    }
    std::cout << (aucun ? " AUCUN" : "") << '\n';
    std::cout << "HORS_SEANCE " << horsSeance << '\n';
    return 0;
}
