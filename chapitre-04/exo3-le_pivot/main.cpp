// Chapitre 4 — Exercice 3 : le pivot.
// Place les quatre coins d'un rectangle tourné autour de son origine.
#include <algorithm>
#include <iostream>
#include <string>

int main() {
    long long n = 0;
    if (!(std::cin >> n)) {
        n = 0;
    }
    long long refuses = 0;

    for (long long i = 0; i < n; ++i) {
        std::string nom;
        long long w = 0;
        long long h = 0;
        long long px = 0;
        long long py = 0;
        long long ox = 0;
        long long oy = 0;
        long long sx = 0;
        long long sy = 0;
        long long angle = 0;
        std::cin >> nom >> w >> h >> px >> py >> ox >> oy >> sx >> sy >> angle;

        if (angle % 90 != 0) {
            std::cout << nom << " ANGLE REFUSE\n";
            ++refuses;
            continue;
        }
        // En C++, -90 % 360 vaut -90 : on ramène le reste dans les positifs.
        long long a = ((angle % 360) + 360) % 360;
        long long c = 1;
        long long s = 0;
        if (a == 90) {
            c = 0;
            s = 1;
        } else if (a == 180) {
            c = -1;
            s = 0;
        } else if (a == 270) {
            c = 0;
            s = -1;
        }

        // Ordre : haut-gauche, haut-droit, bas-droit, bas-gauche.
        const long long lx[4] = {0, w, w, 0};
        const long long ly[4] = {0, 0, h, h};
        long long wx[4];
        long long wy[4];
        for (int k = 0; k < 4; ++k) {
            // L'origine se retire AVANT l'échelle.
            long long ax = (lx[k] - ox) * sx;
            long long ay = (ly[k] - oy) * sy;
            long long rx = ax * c - ay * s;
            long long ry = ax * s + ay * c;
            wx[k] = px + rx;
            wy[k] = py + ry;
        }

        std::cout << nom << " COINS";
        for (int k = 0; k < 4; ++k) {
            std::cout << ' ' << wx[k] << ' ' << wy[k];
        }
        std::cout << '\n';

        // La boîte se calcule après la rotation, sur les quatre coins.
        long long minx = *std::min_element(wx, wx + 4);
        long long maxx = *std::max_element(wx, wx + 4);
        long long miny = *std::min_element(wy, wy + 4);
        long long maxy = *std::max_element(wy, wy + 4);
        std::cout << nom << " BOITE " << minx << ' ' << miny << ' ' << maxx << ' ' << maxy << '\n';
    }

    std::cout << "REFUSES " << refuses << '\n';
    return 0;
}
