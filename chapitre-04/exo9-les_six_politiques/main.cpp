// Chapitre 4 — Exercice 9 : les six politiques de redimensionnement.
// Tout se calcule en entiers 64 bits ; les rapports se comparent en multipliant.
#include <algorithm>
#include <iostream>

struct Resultat {
    const char *nom;
    long long vx;
    long long vy;
    long long vw;
    long long vh;
    long long mw;
    long long mh;
};

// Arrondi au plus proche, une moitié monte : (2a + b) / (2b).
static long long Arrondi(long long a, long long b) {
    return (2 * a + b) / (2 * b);
}

static Resultat Letterbox(const char *nom, long long rw, long long rh, long long w, long long h) {
    Resultat r{nom, 0, 0, 0, 0, rw, rh};
    if (w * rh <= h * rw) {
        r.vw = w;
        r.vh = Arrondi(rh * w, rw);
    } else {
        r.vh = h;
        r.vw = Arrondi(rw * h, rh);
    }
    r.vx = (w - r.vw) / 2;
    r.vy = (h - r.vh) / 2;
    return r;
}

int main() {
    long long rw = 0;
    long long rh = 0;
    long long aw = 0;
    long long ah = 0;
    long long w = 1;
    long long h = 1;
    std::cin >> rw >> rh >> aw >> ah >> w >> h;

    bool reference = (rw != 0 && rh != 0);
    Resultat res[6];

    Resultat follow{"FOLLOW_WINDOW", 0, 0, w, h, w, h};
    res[0] = follow;

    if (!reference) {
        // Sans référence, quatre politiques retombent sur FOLLOW_WINDOW.
        res[1] = Resultat{"STRETCH", 0, 0, w, h, w, h};
        res[2] = Resultat{"FIT_LETTERBOX", 0, 0, w, h, w, h};
        res[3] = Resultat{"INTEGER_SCALE", 0, 0, w, h, w, h};
        res[4] = Resultat{"FIT_CROP", 0, 0, w, h, w, h};
    } else {
        res[1] = Resultat{"STRETCH", 0, 0, w, h, rw, rh};
        res[2] = Letterbox("FIT_LETTERBOX", rw, rh, w, h);

        if (w >= rw && h >= rh) {
            long long k = std::min(w / rw, h / rh);
            Resultat is{"INTEGER_SCALE", 0, 0, rw * k, rh * k, rw, rh};
            is.vx = (w - is.vw) / 2;
            is.vy = (h - is.vh) / 2;
            res[3] = is;
        } else {
            res[3] = Letterbox("INTEGER_SCALE", rw, rh, w, h);
        }

        Resultat crop{"FIT_CROP", 0, 0, w, h, 0, 0};
        if (w * rh > h * rw) {
            crop.mw = rw;
            crop.mh = Arrondi(rw * h, w);
        } else {
            crop.mw = Arrondi(rh * w, h);
            crop.mh = rh;
        }
        res[4] = crop;
    }

    res[5] = Resultat{"MANUAL", 0, 0, aw, ah, aw, ah};

    long long bandes = 0;
    for (int i = 0; i < 6; ++i) {
        const Resultat &r = res[i];
        std::cout << r.nom << ' ' << r.vx << ' ' << r.vy << ' ' << r.vw << ' ' << r.vh << ' ' << r.mw << ' ' << r.mh << '\n';
        // Seul un viewport plus étroit OU plus bas que la fenêtre a des bandes.
        if (r.vw < w || r.vh < h) {
            ++bandes;
        }
    }

    std::cout << "BANDES " << bandes << '\n';
    if (reference && w * rh != h * rw) {
        std::cout << "DEFORMATION OUI\n";
    } else {
        std::cout << "DEFORMATION NON\n";
    }
    return 0;
}
