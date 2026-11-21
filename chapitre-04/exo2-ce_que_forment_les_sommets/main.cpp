// Chapitre 4 — Exercice 2 : ce que forment les sommets.
// Lit N lignes « type sommets » et dit ce que chaque liste dessine vraiment.
#include <iostream>
#include <string>

int main() {
    long long n = 0;
    if (!(std::cin >> n)) {
        n = 0;
    }
    long long totalPoints = 0;
    long long totalSegments = 0;
    long long totalTriangles = 0;
    long long totalRefuses = 0;

    for (long long i = 0; i < n; ++i) {
        std::string type;
        long long s = 0;
        std::cin >> type >> s;

        long long nombre = 0;
        long long restants = 0;
        std::string unite;

        if (type == "POINTS") {
            unite = "POINTS";
            nombre = s;
            restants = 0;
        } else if (type == "LINES") {
            unite = "SEGMENTS";
            nombre = s / 2;
            restants = s % 2;
        } else if (type == "LINE_STRIP") {
            unite = "SEGMENTS";
            if (s >= 2) {
                nombre = s - 1;
                restants = 0;
            } else {
                nombre = 0;
                restants = s;
            }
        } else if (type == "TRIANGLES") {
            unite = "TRIANGLES";
            nombre = s / 3;
            restants = s % 3;
        } else if (type == "TRIANGLE_STRIP" || type == "TRIANGLE_FAN") {
            unite = "TRIANGLES";
            // Moins de trois sommets : aucun triangle, jamais un nombre négatif.
            if (s >= 3) {
                nombre = s - 2;
                restants = 0;
            } else {
                nombre = 0;
                restants = s;
            }
        } else {
            // QUADS et tout type inconnu, comparaison exacte.
            std::cout << type << ' ' << s << " REFUSE\n";
            ++totalRefuses;
            continue;
        }

        if (unite == "POINTS") {
            totalPoints += nombre;
        } else if (unite == "SEGMENTS") {
            totalSegments += nombre;
        } else {
            totalTriangles += nombre;
        }
        std::cout << type << ' ' << s << ' ' << nombre << ' ' << unite << ' ' << restants << '\n';
    }

    std::cout << "POINTS " << totalPoints << '\n';
    std::cout << "SEGMENTS " << totalSegments << '\n';
    std::cout << "TRIANGLES " << totalTriangles << '\n';
    std::cout << "REFUSES " << totalRefuses << '\n';
    return 0;
}
