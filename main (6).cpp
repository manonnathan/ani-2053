// Chapitre 11 — Exercice 5 : compter les gestes.
// Gestes, durée, attente de chaque tâche, et ce que l'attente coûte sur une journée.
#include <cstdio>
#include <iostream>
#include <sstream>
#include <string>

namespace {
    std::string MinSec(long long secondes) {
        char buf[64];
        std::snprintf(buf, sizeof(buf), "%lld:%02lld", secondes / 60, secondes % 60);
        return buf;
    }
}

int main() {
    bool enCours = false;
    std::string nom;
    long long minGestes = 0;
    long long parJour = 0;
    long long debut = 0;
    long long gestes = 0;
    long long attente = 0;
    long long totalSecondes = 0;
    std::string ligne;
    while (std::getline(std::cin, ligne)) {
        std::istringstream in(ligne);
        long long t = 0;
        std::string mot;
        if (!(in >> t >> mot)) {
            continue;
        }
        if (mot == "DEBUT") {
            in >> nom >> minGestes >> parJour;
            enCours = true;
            debut = t;
            gestes = 0;
            attente = 0;
        } else if (mot == "FIN") {
            if (!enCours) {
                continue;
            }
            const long long enTrop = gestes > minGestes ? gestes - minGestes : 0;
            const long long jour = attente * parJour / 1000;  // secondes entières, arrondi inférieur
            totalSecondes += jour;
            std::cout << nom << " GESTES " << gestes << " EN_TROP " << enTrop << " DUREE " << (t - debut)
                      << " ATTENTE " << attente << " JOUR " << MinSec(jour) << '\n';
            enCours = false;
        } else if (mot == "ATTENTE") {
            long long d = 0;
            in >> d;
            if (enCours) {
                attente += d;  // une attente n'est pas un geste
            }
        } else if (mot == "CLIC" || mot == "TOUCHE" || mot == "MOLETTE" || mot == "GLISSE") {
            if (enCours) {
                ++gestes;  // hors de toute tâche : ignoré
            }
        }
    }
    if (enCours) {
        std::cout << nom << " INACHEVEE\n";
    }
    std::cout << "TOTAL_JOUR " << MinSec(totalSecondes) << '\n';
    return 0;
}
