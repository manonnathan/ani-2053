// Chapitre 9 — Exercice 2 : les promesses d'une signature.
// Une vue const : seules les méthodes qui promettent const peuvent être appelées.
#include <cstdio>

#include "NkRef/NkRefView.h"

int main() {
    const nkref::NkRefView vue{};
    const nkentseu::math::NkVec2f viewport{800.0f, 600.0f};
    const nkentseu::math::NkVec2f monde = vue.PixelToWorld({100.0f, 50.0f}, viewport);
    const float pas = vue.GridSpacing(32.0f);
    std::printf("PixelToWorld(100, 50) = (%.2f, %.2f)\n", monde.x, monde.y);
    std::printf("GridSpacing(32) = %.2f\n", pas);
    // Ligne refusée par le compilateur : PanByPixels n'a pas de const final, la vue est const.
    // vue.PanByPixels(1.0f, 0.0f);
    return 0;
}
