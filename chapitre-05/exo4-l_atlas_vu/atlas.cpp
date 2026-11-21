// Chapitre 5 — Exercice 4 : l'atlas, vu.
// Usage : atlas police.ttf [taille]     (18 par défaut ; relancer avec 48)
// Construit l'atlas, l'enregistre en PNG, compte les glyphes et mesure la place perdue.
#include <cstdio>
#include <cstdlib>
#include <new>

#include "NKFont/NkFont.h"
#include "NKImage/Core/NkImage.h"

using namespace nkentseu;

int main(int argc, char **argv) {
    const char *police = (argc > 1) ? argv[1] : "Resources/Fonts/Cousine-Regular.ttf";
    float taille = (argc > 2) ? static_cast<float>(std::atof(argv[2])) : 18.f;

    NkFontAtlas atlas;
    NkFont *font = atlas.AddFontFromFile(police, taille);
    if (font == nullptr) {
        std::printf("police introuvable : %s\n", police);
        return 1;
    }
    // Build rastérise tout d'un coup : après l'ajout de la police, jamais avant.
    if (!atlas.Build()) {
        std::printf("Build a echoue\n");
        return 1;
    }

    nkft_uint8 *pixels = nullptr;
    nkft_int32 largeur = 0;
    nkft_int32 hauteur = 0;
    atlas.GetTexDataAsRGBA32(&pixels, &largeur, &hauteur);

    // Wrap ne copie pas : l'atlas doit vivre jusqu'à la fin de SavePNG.
    NkImage vue = NkImage::Wrap(pixels, largeur, hauteur, NkImagePixelFormat::NK_RGBA32);
    char nom[64];
    std::snprintf(nom, sizeof(nom), "atlas-%d.png", static_cast<int>(taille));
    vue.SavePNG(nom);

    // Place perdue : part des pixels où aucun glyphe n'est dessiné (alpha nul).
    long long vides = 0;
    const long long total = static_cast<long long>(largeur) * hauteur;
    for (long long i = 0; i < total; ++i) {
        if (pixels[i * 4 + 3] == 0) {
            ++vides;
        }
    }
    std::printf("taille : %d, glyphes : %u, texture : %d %d, perdu : %lld\n", static_cast<int>(taille),
                font->glyphCount, largeur, hauteur, vides * 100 / total);
    return 0;
}
