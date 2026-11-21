// Chapitre 5 — Exercice 6 : la ligne de base.
// Pose un texte glyphe par glyphe sur sa ligne de base, avec le crénage.
#include <algorithm>
#include <climits>
#include <iostream>
#include <map>
#include <string>

struct Glyphe {
    long long avance = 0;
    long long x0 = 0;
    long long y0 = 0;
    long long x1 = 0;
    long long y1 = 0;
};

int main() {
    long long g = 0;
    std::cin >> g;
    std::map<char, Glyphe> table;
    for (long long i = 0; i < g; ++i) {
        std::string c;
        Glyphe gl;
        std::cin >> c >> gl.avance >> gl.x0 >> gl.y0 >> gl.x1 >> gl.y1;
        table[c[0]] = gl;
    }

    long long k = 0;
    std::cin >> k;
    std::map<std::string, long long> crenage;
    for (long long i = 0; i < k; ++i) {
        std::string couple;
        long long correction = 0;
        std::cin >> couple >> correction;
        crenage[couple] = correction;
    }

    std::string texte;
    long long ox = 0;
    long long oy = 0;
    std::cin >> texte >> ox >> oy;

    long long x = ox;
    bool dessine = false;
    long long minx = LLONG_MAX;
    long long miny = LLONG_MAX;
    long long maxx = LLONG_MIN;
    long long maxy = LLONG_MIN;
    long long absents = 0;

    for (size_t i = 0; i < texte.size(); ++i) {
        char c = texte[i];
        auto it = table.find(c);
        if (it == table.end()) {
            // Absent : n'avance pas, et aucun crénage ne se cherche après lui.
            std::cout << c << " ABSENT\n";
            ++absents;
            continue;
        }
        const Glyphe &gl = it->second;
        std::cout << c << ' ' << x << '\n';

        if (gl.x1 > gl.x0 && gl.y1 > gl.y0) {
            dessine = true;
            minx = std::min(minx, x + gl.x0);
            miny = std::min(miny, oy + gl.y0);
            maxx = std::max(maxx, x + gl.x1);
            maxy = std::max(maxy, oy + gl.y1);
        }
        x = x + gl.avance;
        if (i + 1 < texte.size()) {
            std::string couple;
            couple.push_back(c);
            couple.push_back(texte[i + 1]);
            auto kt = crenage.find(couple);
            if (kt != crenage.end()) {
                x = x + kt->second;
            }
        }
    }

    std::cout << "CURSEUR " << x << '\n';
    if (!dessine) {
        std::cout << "BOITE AUCUNE\n";
        std::cout << "MONTE 0\n";
        std::cout << "DESCEND 0\n";
        std::cout << "ECRAN RIEN\n";
    } else {
        std::cout << "BOITE " << minx << ' ' << miny << ' ' << maxx << ' ' << maxy << '\n';
        std::cout << "MONTE " << (miny < oy ? oy - miny : 0) << '\n';
        std::cout << "DESCEND " << (maxy > oy ? maxy - oy : 0) << '\n';
        if (maxy <= 0) {
            std::cout << "ECRAN HORS\n";
        } else if (miny < 0) {
            std::cout << "ECRAN COUPE\n";
        } else {
            std::cout << "ECRAN VISIBLE\n";
        }
    }
    std::cout << "ABSENTS " << absents << '\n';
    return 0;
}
