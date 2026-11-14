// Chapitre 10 — Exercice 5 : le hit test à rebours.
// L'ordre du tableau est la profondeur : on cherche de la dernière image à la première.
#include <cmath>
#include <iostream>
#include <string>
#include <vector>

namespace {
    struct Image {
        std::string nom;
        double x = 0, y = 0, largeur = 0, hauteur = 0, echelle = 1;

        bool Touche(double px, double py) const {
            // La taille lue est native : l'échelle donne la taille dans le monde. Le bord touche.
            double dl = largeur * echelle / 2.0;
            double dh = hauteur * echelle / 2.0;
            return std::fabs(px - x) <= dl && std::fabs(py - y) <= dh;
        }
    };
}

int main() {
    int n = 0;
    std::cin >> n;
    std::vector<Image> planche(n > 0 ? n : 0);
    for (Image &im : planche) {
        std::cin >> im.nom >> im.x >> im.y >> im.largeur >> im.hauteur >> im.echelle;
    }
    int q = 0;
    std::cin >> q;
    int erreurs = 0;
    for (int k = 1; k <= q; ++k) {
        double px = 0, py = 0;
        std::cin >> px >> py;
        std::string a = "AUCUN";
        for (int i = static_cast<int>(planche.size()) - 1; i >= 0; --i) {
            if (planche[i].Touche(px, py)) {
                a = planche[i].nom;
                break;
            }
        }
        std::string b = "AUCUN";
        for (const Image &im : planche) {
            if (im.Touche(px, py)) {
                b = im.nom;
                break;
            }
        }
        erreurs += (a != b) ? 1 : 0;
        std::cout << "CLIC " << k << ' ' << a << ' ' << b << '\n';
    }
    std::cout << "ERREURS " << erreurs << '\n';
    return 0;
}
