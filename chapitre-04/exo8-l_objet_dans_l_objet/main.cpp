// Chapitre 4 — Exercice 8 : l'objet dans l'objet.
// Compose les transformations d'une hiérarchie : échelle, puis rotation, puis translation du parent.
#include <iostream>
#include <map>
#include <string>

struct Monde {
    long long x = 0;
    long long y = 0;
    long long angle = 0;
    long long echelle = 1;
    long long niveau = 1;
};

static long long Normalise(long long angle) {
    return ((angle % 360) + 360) % 360;
}

static void CosSin(long long angle, long long &c, long long &s) {
    long long a = Normalise(angle);
    c = 1;
    s = 0;
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
}

int main() {
    long long n = 0;
    if (!(std::cin >> n)) {
        n = 0;
    }
    std::map<std::string, Monde> objets;
    long long profondeur = 0;

    for (long long i = 0; i < n; ++i) {
        std::string nom;
        std::string parent;
        long long tx = 0;
        long long ty = 0;
        long long angle = 0;
        long long echelle = 1;
        std::cin >> nom >> parent >> tx >> ty >> angle >> echelle;

        Monde m;
        if (parent == "-") {
            m.x = tx;
            m.y = ty;
            m.angle = Normalise(angle);
            m.echelle = echelle;
            m.niveau = 1;
        } else {
            // On emploie l'état du parent DANS LE MONDE, pas sa ligne.
            const Monde &pm = objets[parent];
            long long ax = tx * pm.echelle;
            long long ay = ty * pm.echelle;
            long long c = 1;
            long long s = 0;
            CosSin(pm.angle, c, s);
            long long rx = ax * c - ay * s;
            long long ry = ax * s + ay * c;
            m.x = pm.x + rx;
            m.y = pm.y + ry;
            m.angle = Normalise(pm.angle + angle);
            m.echelle = pm.echelle * echelle;
            m.niveau = pm.niveau + 1;
        }
        objets[nom] = m;
        if (m.niveau > profondeur) {
            profondeur = m.niveau;
        }
        std::cout << nom << ' ' << m.x << ' ' << m.y << ' ' << m.angle << ' ' << m.echelle << '\n';
    }

    std::cout << "PROFONDEUR " << profondeur << '\n';
    return 0;
}
