// Chapitre 5 — Exercice 5 : trois polices, un atlas.
// Range des glyphes par étagères et agrandit la texture tant qu'ils ne tiennent pas.
#include <iostream>
#include <string>
#include <vector>

struct Groupe {
    std::string police;
    long long n = 0;
    long long w = 0;
    long long h = 0;
    long long x1 = 0;
    long long y1 = 0;
    long long x2 = 0;
    long long y2 = 0;
};

// Un essai complet, depuis le premier glyphe. Rend faux si un glyphe ne tient pas.
static bool Essai(std::vector<Groupe> &groupes, long long p, long long largeur, long long hauteur) {
    long long x = p;
    long long y = p;
    long long e = 0;
    for (Groupe &g : groupes) {
        for (long long i = 0; i < g.n; ++i) {
            long long rw = g.w + p;
            long long rh = g.h + p;
            if (x + rw > largeur - p) {
                x = p;
                y = y + e + p;
                e = 0;
                if (x + rw > largeur - p) {
                    return false;
                }
            }
            if (y + rh > hauteur - p) {
                return false;
            }
            if (i == 0) {
                g.x1 = x;
                g.y1 = y;
            }
            g.x2 = x;
            g.y2 = y;
            x = x + rw;
            if (rh > e) {
                e = rh;
            }
        }
    }
    return true;
}

int main() {
    long long p = 0;
    long long l = 0;
    long long nbGroupes = 0;
    std::cin >> p >> l >> nbGroupes;

    std::vector<Groupe> groupes;
    for (long long i = 0; i < nbGroupes; ++i) {
        Groupe g;
        std::cin >> g.police >> g.n >> g.w >> g.h;
        groupes.push_back(g);
    }
    if (groupes.empty()) {
        std::cout << "AUCUN\n";
        return 0;
    }

    long long largeur = l;
    if (l == 0) {
        largeur = 512;
        long long besoin = 0;
        for (const Groupe &g : groupes) {
            besoin += g.n * (g.w + p) * (g.h + p);
        }
        while (largeur * largeur < 2 * besoin && largeur < 4096) {
            largeur *= 2;
        }
    }
    long long hauteur = largeur;

    int essai = 1;
    bool reussi = false;
    for (essai = 1; essai <= 8; ++essai) {
        if (Essai(groupes, p, largeur, hauteur)) {
            reussi = true;
            break;
        }
        if (essai == 8) {
            break;
        }
        if (largeur == hauteur) {
            largeur *= 2;
        } else {
            hauteur = largeur;
        }
    }

    if (!reussi) {
        std::cout << "ESSAIS 8\n";
        std::cout << "ECHEC\n";
        return 0;
    }

    long long occupe = 0;
    for (const Groupe &g : groupes) {
        occupe += g.n * g.w * g.h;
    }
    long long aire = largeur * hauteur;
    std::cout << "ESSAIS " << essai << '\n';
    std::cout << "TEXTURE " << largeur << ' ' << hauteur << '\n';
    for (const Groupe &g : groupes) {
        std::cout << g.police << ' ' << g.x1 << ' ' << g.y1 << ' ' << g.x2 << ' ' << g.y2 << '\n';
    }
    std::cout << "OCCUPE " << occupe << '\n';
    std::cout << "PERDU " << (aire - occupe) * 100 / aire << '\n';
    return 0;
}
