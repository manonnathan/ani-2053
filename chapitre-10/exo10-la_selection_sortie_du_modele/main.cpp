// Chapitre 10 — Exercice 10 : la sélection sortie du modèle.
// Trois façons de tenir la sélection, trois planches séparées : lesquelles se trompent d'image ?
#include <algorithm>
#include <iostream>
#include <set>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

namespace {
    struct Op {
        std::string quoi;
        std::string nom;
        long i = -1;
        long j = -1;
    };

    using Liste = std::vector<std::string>;

    bool Existe(long i, size_t taille) {
        return i >= 0 && static_cast<size_t>(i) < taille;
    }

    // DANS_ITEM : un drapeau dans chaque image, qui la suit partout.
    void DansItem(const std::vector<Op> &ops, Liste &planche, Liste &sel) {
        std::vector<std::pair<std::string, bool>> p;
        for (const Op &o : ops) {
            if (o.quoi == "AJOUTE") {
                p.emplace_back(o.nom, false);
            } else if (o.quoi == "CLIC") {
                for (auto &x : p) {
                    x.second = false;
                }
                if (Existe(o.i, p.size())) {
                    p[o.i].second = true;
                }
            } else if (o.quoi == "AJOUTE_SEL") {
                if (Existe(o.i, p.size())) {
                    p[o.i].second = true;
                }
            } else if (o.quoi == "ECHANGE") {
                if (Existe(o.i, p.size()) && Existe(o.j, p.size())) {
                    std::swap(p[o.i], p[o.j]);
                }
            } else if (o.quoi == "RETIRE") {
                if (Existe(o.i, p.size())) {
                    p.erase(p.begin() + o.i);
                }
            } else if (o.quoi == "SUPPRIME") {
                p.erase(std::remove_if(p.begin(), p.end(), [](const auto &x) { return x.second; }), p.end());
            }
        }
        for (const auto &x : p) {
            planche.push_back(x.first);
            if (x.second) {
                sel.push_back(x.first);
            }
        }
    }

    // PAR_INDEX : l'interface garde des index, que personne ne met à jour.
    void ParIndex(const std::vector<Op> &ops, Liste &planche, Liste &sel) {
        Liste p;
        std::set<long> gardes;
        for (const Op &o : ops) {
            if (o.quoi == "AJOUTE") {
                p.push_back(o.nom);
            } else if (o.quoi == "CLIC") {
                gardes.clear();
                if (Existe(o.i, p.size())) {
                    gardes.insert(o.i);
                }
            } else if (o.quoi == "AJOUTE_SEL") {
                if (Existe(o.i, p.size())) {
                    gardes.insert(o.i);
                }
            } else if (o.quoi == "ECHANGE") {
                if (Existe(o.i, p.size()) && Existe(o.j, p.size())) {
                    std::swap(p[o.i], p[o.j]);
                }
            } else if (o.quoi == "RETIRE") {
                if (Existe(o.i, p.size())) {
                    p.erase(p.begin() + o.i);
                }
            } else if (o.quoi == "SUPPRIME") {
                Liste reste;
                for (size_t k = 0; k < p.size(); ++k) {
                    if (gardes.count(static_cast<long>(k)) == 0) {
                        reste.push_back(p[k]);
                    }
                }
                p = reste;
                gardes.clear();
            }
        }
        for (size_t k = 0; k < p.size(); ++k) {
            planche.push_back(p[k]);
            if (gardes.count(static_cast<long>(k)) != 0) {
                sel.push_back(p[k]);
            }
        }
    }

    // PAR_ID : l'interface garde des noms.
    void ParId(const std::vector<Op> &ops, Liste &planche, Liste &sel) {
        Liste p;
        std::set<std::string> gardes;
        for (const Op &o : ops) {
            if (o.quoi == "AJOUTE") {
                p.push_back(o.nom);
            } else if (o.quoi == "CLIC") {
                gardes.clear();
                if (Existe(o.i, p.size())) {
                    gardes.insert(p[o.i]);
                }
            } else if (o.quoi == "AJOUTE_SEL") {
                if (Existe(o.i, p.size())) {
                    gardes.insert(p[o.i]);
                }
            } else if (o.quoi == "ECHANGE") {
                if (Existe(o.i, p.size()) && Existe(o.j, p.size())) {
                    std::swap(p[o.i], p[o.j]);
                }
            } else if (o.quoi == "RETIRE") {
                if (Existe(o.i, p.size())) {
                    p.erase(p.begin() + o.i);
                }
            } else if (o.quoi == "SUPPRIME") {
                Liste reste;
                for (const std::string &nom : p) {
                    if (gardes.count(nom) == 0) {
                        reste.push_back(nom);
                    }
                }
                p = reste;
                gardes.clear();
            }
        }
        for (const std::string &nom : p) {
            planche.push_back(nom);
            if (gardes.count(nom) != 0) {
                sel.push_back(nom);
            }
        }
    }

    void Afficher(const std::string &c, const char *quoi, const Liste &l) {
        std::cout << c << ' ' << quoi;
        if (l.empty()) {
            std::cout << " AUCUN";
        }
        for (const std::string &n : l) {
            std::cout << ' ' << n;
        }
        std::cout << '\n';
    }
}

int main() {
    std::vector<Op> ops;
    std::string ligne;
    while (std::getline(std::cin, ligne)) {
        std::istringstream in(ligne);
        Op o;
        if (!(in >> o.quoi)) {
            continue;
        }
        if (o.quoi == "AJOUTE") {
            in >> o.nom;
        } else if (o.quoi == "ECHANGE") {
            in >> o.i >> o.j;
        } else if (o.quoi == "CLIC" || o.quoi == "AJOUTE_SEL" || o.quoi == "RETIRE") {
            in >> o.i;
        }
        ops.push_back(o);
    }
    Liste p[3], s[3];
    DansItem(ops, p[0], s[0]);
    ParIndex(ops, p[1], s[1]);
    ParId(ops, p[2], s[2]);
    const char *nom[3] = {"DANS_ITEM", "PAR_INDEX", "PAR_ID"};
    for (int c = 0; c < 3; ++c) {
        Afficher(nom[c], "PLANCHE", p[c]);
        Afficher(nom[c], "SELECTION", s[c]);
    }
    Liste faux;
    for (int c = 1; c < 3; ++c) {
        if (p[c] != p[0] || s[c] != s[0]) {
            faux.push_back(nom[c]);
        }
    }
    std::cout << "FAUX";
    if (faux.empty()) {
        std::cout << " AUCUN";
    }
    for (const std::string &f : faux) {
        std::cout << ' ' << f;
    }
    std::cout << '\n';
    return 0;
}
