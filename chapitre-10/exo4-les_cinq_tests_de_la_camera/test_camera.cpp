// Chapitre 10 — Exercice 4 : les cinq tests de la caméra.
// La vraie caméra de NkRef, sans aucune fenêtre : elle se teste comme une calculatrice.
#include <cmath>
#include <cstdio>

#include "NkRef/NkRefView.h"

using nkentseu::math::NkVec2f;
using nkref::NkRefView;

namespace {
    const NkVec2f kViewport{800.0f, 600.0f};

    // Deux flottants se comparent à une tolérance près, jamais par ==.
    bool Proche(float a, float b, float tol = 0.001f) {
        return std::fabs(a - b) <= tol;
    }

    // WorldToPixel(PixelToWorld(p)) redonne p à 0,001 près.
    bool aller_retour() {
        NkRefView v;
        v.center = {12.5f, -40.0f};
        v.zoom = 2.0f;
        const NkVec2f points[] = {{0.0f, 0.0f}, {123.4f, 56.7f}, {799.0f, 599.0f}, {-50.0f, 900.0f}};
        for (const NkVec2f &p : points) {
            const NkVec2f q = v.WorldToPixel(v.PixelToWorld(p, kViewport), kViewport);
            if (!Proche(q.x, p.x) || !Proche(q.y, p.y)) {
                return false;
            }
        }
        return true;
    }

    // Après ZoomAtPixel(f, p, vp), PixelToWorld(p) donne le même point du monde qu'avant.
    bool point_fixe() {
        NkRefView v;
        v.center = {30.0f, 20.0f};
        const NkVec2f p{100.0f, 50.0f};
        const float facteurs[] = {2.0f, 1.7f, 0.5f, 3.0f};
        for (float f : facteurs) {
            const NkVec2f avant = v.PixelToWorld(p, kViewport);
            v.ZoomAtPixel(f, p, kViewport);
            const NkVec2f apres = v.PixelToWorld(p, kViewport);
            if (!Proche(avant.x, apres.x) || !Proche(avant.y, apres.y)) {
                return false;
            }
        }
        return true;
    }

    // Un zoom de facteur 1000, puis un de facteur 0,00001, laissent le zoom dans les bornes.
    bool bornes() {
        NkRefView v;
        v.ZoomAtPixel(1000.0f, {400.0f, 300.0f}, kViewport);
        if (v.zoom < NkRefView::kZoomMin || v.zoom > NkRefView::kZoomMax) {
            return false;
        }
        v.ZoomAtPixel(0.00001f, {400.0f, 300.0f}, kViewport);
        return v.zoom >= NkRefView::kZoomMin && v.zoom <= NkRefView::kZoomMax;
    }

    // Quand le zoom diminue, GridSpacing(m) ne diminue jamais.
    bool grille() {
        NkRefView v;
        v.zoom = NkRefView::kZoomMax;
        float precedent = v.GridSpacing(32.0f);
        while (v.zoom > NkRefView::kZoomMin) {
            v.zoom *= 0.9f;
            if (v.zoom < NkRefView::kZoomMin) {
                v.zoom = NkRefView::kZoomMin;
            }
            const float s = v.GridSpacing(32.0f);
            if (s < precedent) {
                return false;
            }
            precedent = s;
        }
        return true;
    }

    // Après un zoom, un déplacement et Reset(), le centre vaut (0, 0) et le zoom 1.
    bool origine() {
        NkRefView v;
        v.ZoomAtPixel(3.0f, {10.0f, 10.0f}, kViewport);
        v.PanByPixels(120.0f, -45.0f);
        v.Reset();
        return v.center.x == 0.0f && v.center.y == 0.0f && v.zoom == 1.0f;  // valeurs exactes posées par Reset
    }
}

int main() {
    struct Test {
        const char *nom;
        bool (*fn)();
    };
    const Test tests[] = {{"aller_retour", aller_retour}, {"point_fixe", point_fixe}, {"bornes", bornes},
                          {"grille", grille}, {"origine", origine}};
    int echecs = 0;
    for (const Test &t : tests) {
        const bool ok = t.fn();
        echecs += ok ? 0 : 1;
        std::printf("TEST %s : %s\n", t.nom, ok ? "OK" : "ECHEC");
    }
    return echecs;
}
