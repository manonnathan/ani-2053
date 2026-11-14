// Chapitre 10 — Exercice 3 : l'aller-retour.
// La caméra d'un canevas infini : un centre et un zoom. La taille de la fenêtre n'y est pas
// stockée : elle est reçue une fois, puis passée à chaque calcul.
#include <cstdio>
#include <iostream>
#include <string>

namespace {
    struct Viewport {
        double w = 0.0;
        double h = 0.0;
    };

    struct Camera {
        double cx = 0.0;
        double cy = 0.0;
        double zoom = 1.0;  // pixels par unité du monde

        static constexpr double kZoomMin = 0.02;
        static constexpr double kZoomMax = 64.0;

        void PixelToWorld(double px, double py, const Viewport &vp, double &mx, double &my) const {
            mx = cx + (px - vp.w / 2.0) / zoom;
            my = cy + (py - vp.h / 2.0) / zoom;
        }
        void WorldToPixel(double mx, double my, const Viewport &vp, double &px, double &py) const {
            px = (mx - cx) * zoom + vp.w / 2.0;
            py = (my - cy) * zoom + vp.h / 2.0;
        }
        void PanByPixels(double dx, double dy) {
            cx -= dx / zoom;  // glisser vers la droite fait reculer le centre
            cy -= dy / zoom;
        }
        void ZoomAtPixel(double f, double px, double py, const Viewport &vp) {
            double ax = 0.0;
            double ay = 0.0;
            PixelToWorld(px, py, vp, ax, ay);  // AVANT de changer le zoom
            zoom *= f;
            if (zoom < kZoomMin) {
                zoom = kZoomMin;
            }
            if (zoom > kZoomMax) {
                zoom = kZoomMax;
            }
            cx = ax - (px - vp.w / 2.0) / zoom;
            cy = ay - (py - vp.h / 2.0) / zoom;
        }
        double GridSpacing(double m) const {
            double s = 1.0;
            while (s * zoom < m) {
                s *= 2.0;
            }
            while (s * 0.5 * zoom >= m) {
                s *= 0.5;
            }
            return s;
        }
        void Reset() {
            cx = 0.0;
            cy = 0.0;
            zoom = 1.0;
        }
    };

    // Arrondi au plus proche ; -0.00 s'écrit 0.00.
    std::string Nombre(double v, int chiffres) {
        char buf[64];
        std::snprintf(buf, sizeof(buf), "%.*f", chiffres, v);
        std::string s = buf;
        if (!s.empty() && s[0] == '-' && s.find_first_not_of("0.", 1) == std::string::npos) {
            s.erase(0, 1);
        }
        return s;
    }
}

int main() {
    Viewport vp;
    std::cin >> vp.w >> vp.h;
    Camera cam;
    std::string cmd;
    while (std::cin >> cmd) {
        if (cmd == "VERS_MONDE") {
            double px = 0, py = 0, mx = 0, my = 0;
            std::cin >> px >> py;
            cam.PixelToWorld(px, py, vp, mx, my);
            std::cout << "MONDE " << Nombre(mx, 2) << ' ' << Nombre(my, 2) << '\n';
        } else if (cmd == "VERS_PIXEL") {
            double mx = 0, my = 0, px = 0, py = 0;
            std::cin >> mx >> my;
            cam.WorldToPixel(mx, my, vp, px, py);
            std::cout << "PIXEL " << Nombre(px, 2) << ' ' << Nombre(py, 2) << '\n';
        } else if (cmd == "DEPLACE") {
            double dx = 0, dy = 0;
            std::cin >> dx >> dy;
            cam.PanByPixels(dx, dy);
        } else if (cmd == "ZOOM") {
            double f = 1, px = 0, py = 0;
            std::cin >> f >> px >> py;
            cam.ZoomAtPixel(f, px, py, vp);
        } else if (cmd == "GRILLE") {
            double m = 1;
            std::cin >> m;
            std::cout << "GRILLE " << Nombre(cam.GridSpacing(m), 6) << '\n';
        } else if (cmd == "ORIGINE") {
            cam.Reset();
        }
    }
    std::cout << "CENTRE " << Nombre(cam.cx, 2) << ' ' << Nombre(cam.cy, 2) << '\n';
    std::cout << "ZOOM " << Nombre(cam.zoom, 2) << '\n';
    return 0;
}
