// Chapitre 11 — Exercice 9 : la matrice.
// Range une liste de manques dans les quatre cases valeur/coût, écarte ce qui n'y a pas sa place,
// et dit si la liste ose refuser quelque chose.
#include <algorithm>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <vector>

namespace {
    struct Entree {
        std::string nom;
        int valeur = 1;
        int cout = 1;
        std::string etiquette;
        std::string genre;
        std::string cible;
        long ordre = 0;
        std::string raison;  // vide = gardée
        int caseRang = 0;
    };

    const char *const kCase[] = {"MAINTENANT", "DECOUPER", "EN_PASSANT", "ON_NE_LE_FAIT_PAS"};

    int RangEtiquette(const std::string &e) {
        return e == "MESUREE" ? 0 : (e == "OBSERVEE" ? 1 : 2);
    }
}

int main() {
    long n = 0;
    std::cin >> n;
    std::string reste;
    std::getline(std::cin, reste);
    std::vector<Entree> liste;
    for (long i = 0; i < n; ++i) {
        std::string ligne;
        if (!std::getline(std::cin, ligne)) {
            break;
        }
        std::istringstream in(ligne);
        Entree e;
        in >> e.nom >> e.valeur >> e.cout >> e.etiquette >> e.genre;
        if (e.genre == "SOCLE") {
            in >> e.cible;
        }
        e.ordre = i;
        if (e.genre == "REECRITURE") {
            e.raison = "reecriture";
        } else if (e.genre == "CHOIX") {
            e.raison = "choix";
        } else if (e.etiquette == "-") {
            e.raison = "sans_etiquette";
        }
        liste.push_back(e);
    }
    // La valeur d'un socle : celle de sa cible, si elle est plus forte et si la cible est un MANQUE gardé.
    std::map<std::string, const Entree *> parNom;
    for (const Entree &e : liste) {
        parNom[e.nom] = &e;
    }
    std::vector<int> valeur(liste.size());
    for (size_t i = 0; i < liste.size(); ++i) {
        valeur[i] = liste[i].valeur;
        if (liste[i].genre == "SOCLE") {
            auto it = parNom.find(liste[i].cible);
            if (it != parNom.end() && it->second->genre == "MANQUE" && it->second->raison.empty()) {
                valeur[i] = std::max(valeur[i], it->second->valeur);
            }
        }
    }
    std::vector<size_t> gardees;
    bool refus = false;
    for (size_t i = 0; i < liste.size(); ++i) {
        if (!liste[i].raison.empty()) {
            continue;
        }
        const bool forte = valeur[i] >= 3;
        const bool eleve = liste[i].cout >= 3;
        liste[i].caseRang = forte ? (eleve ? 1 : 0) : (eleve ? 3 : 2);
        refus = refus || liste[i].caseRang == 3;
        gardees.push_back(i);
    }
    std::stable_sort(gardees.begin(), gardees.end(), [&](size_t a, size_t b) {
        const Entree &x = liste[a];
        const Entree &y = liste[b];
        if (x.caseRang != y.caseRang) {
            return x.caseRang < y.caseRang;
        }
        if (RangEtiquette(x.etiquette) != RangEtiquette(y.etiquette)) {
            return RangEtiquette(x.etiquette) < RangEtiquette(y.etiquette);
        }
        if (valeur[a] != valeur[b]) {
            return valeur[a] > valeur[b];
        }
        if (x.cout != y.cout) {
            return x.cout < y.cout;
        }
        return x.ordre < y.ordre;
    });
    for (size_t i : gardees) {
        std::cout << kCase[liste[i].caseRang] << ' ' << liste[i].nom << ' ' << liste[i].etiquette << '\n';
    }
    for (const Entree &e : liste) {
        if (!e.raison.empty()) {
            std::cout << "HORS_LISTE " << e.nom << ' ' << e.raison << '\n';
        }
    }
    std::cout << "VERDICT " << (refus ? "HONNETE" : "COMPLAISANT") << '\n';
    return 0;
}
