// Chapitre 10 — Exercice 8 : le test d'aller-retour du format.
// Écrire, relire, réécrire, comparer les octets : tout ce qui ne se conserve pas tombe.
#include <cstdio>

#include "NkRef/NkRefFile.h"

using nkentseu::uint8;
using nkentseu::usize;
using nkentseu::NkString;
using nkentseu::NkVector;
using namespace nkref;

int main() {
    // ── La planche : deux images, un trait, une vue déplacée ─────────────────
    NkRefBoard planche;
    planche.AddItem({-64.0f, 32.0f}, 128, 96, NkString("C:/refs/arbre.png"));
    planche.AddItem({100.0f, -20.5f}, 64, 48, NkString(""));
    planche.items[0].scale = 0.5f;
    planche.items[0].rotationDeg = 90.0f;
    planche.items[0].opacity = 0.75f;
    planche.items[0].mirrorX = true;
    planche.items[1].scale = 2.0f;
    planche.items[1].rotationDeg = -30.0f;
    planche.items[1].mirrorY = true;
    planche.BeginStroke(255, 0, 0, 3.0f, {0.0f, 0.0f});
    planche.AppendStrokePoint({10.0f, 5.0f}, 0.0f);
    planche.AppendStrokePoint({25.0f, -7.5f}, 0.0f);

    // Des octets source inventés (le format les recopie tels quels).
    const uint8 octetsA[] = {0x89, 'P', 'N', 'G', 1, 2, 3, 4, 5};
    const uint8 octetsB[] = {0xFF, 0xD8, 0xFF, 0xE0, 42, 43};
    const NkRefFileImage images[2] = {{octetsA, sizeof(octetsA)}, {octetsB, sizeof(octetsB)}};

    NkRefView vue;
    vue.center = {12.5f, -40.0f};
    vue.zoom = 2.0f;

    // ── 1. Écrire ────────────────────────────────────────────────────────────
    NkVector<uint8> premier;
    NkRefSerialize(premier, planche, images, vue, true, false);

    // ── 2. Relire ────────────────────────────────────────────────────────────
    NkVector<NkRefLoadedItem> items;
    NkVector<NkRefStroke> traits;
    NkRefView vueLue;
    bool sombre = false;
    bool grille = true;
    const bool relu = NkRefDeserialize(premier.Data(), premier.Size(), items, traits, vueLue, sombre, grille);

    // ── 3. Reconstruire une planche avec ce qui a été relu, et réécrire ─────
    NkRefBoard reconstruite;
    NkVector<NkRefFileImage> imagesRelues;
    for (usize i = 0; i < items.Size(); ++i) {
        reconstruite.items.PushBack(items[i].item);
    }
    for (usize i = 0; i < items.Size(); ++i) {  // après les PushBack : les adresses ne bougent plus
        imagesRelues.PushBack({items[i].src.Data(), items[i].src.Size()});
    }
    for (usize i = 0; i < traits.Size(); ++i) {
        reconstruite.strokes.PushBack(traits[i]);
    }
    NkVector<uint8> second;
    NkRefSerialize(second, reconstruite, imagesRelues.Data(), vueLue, sombre, grille);

    // ── 4. Comparer, octet par octet ─────────────────────────────────────────
    bool identiques = premier.Size() == second.Size();
    usize premierEcart = premier.Size();
    for (usize i = 0; identiques && i < premier.Size(); ++i) {
        if (premier[i] != second[i]) {
            identiques = false;
            premierEcart = i;
        }
    }
    std::printf("octets : %u, relu : %s, identiques : %s", static_cast<unsigned>(premier.Size()), relu ? "OUI" : "NON",
                identiques ? "OUI" : "NON");
    if (!identiques) {
        std::printf(" (second : %u octets, premier ecart a l'octet %u)", static_cast<unsigned>(second.Size()),
                    static_cast<unsigned>(premierEcart));
    }
    std::printf("\n");
    return identiques && relu ? 0 : 1;
}
