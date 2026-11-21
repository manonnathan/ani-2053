// Chapitre 4 — Exercice 4 : à la main.
// Le même carré rouge, à 100 pixels par seconde, sans coquille : fenêtre, cible et boucle écrites ici.
#include "NKCanvas/Core/NkContextDesc.h"
#include "NKCanvas/Core/NkGraphicsApi.h"
#include "NKCanvas/Renderer/Shapes/NkRectangleShape.h"
#include "NKCanvas/Renderer/Targets/NkRenderWindow.h"
#include "NKEvent/NkWindowEvent.h"
#include "NKTime/NkTime.h"
#include "NKWindow/Core/NkWindowConfig.h"
#include "NKWindow/NKMain.h"
#include "NKWindow/NKWindow.h"

using namespace nkentseu;
using namespace nkentseu::renderer;

int nkmain(const NkEntryState &state) {
    (void)state;
    NkWindowConfig cfg;
    cfg.title = "A la main";
    cfg.width = 800;
    cfg.height = 450;
    cfg.centered = true;

    NkWindow fenetre;
    fenetre.Create(cfg);
    if (!fenetre.IsOpen()) {
        return -1;
    }
    // La fenêtre ne dessine pas : la cible de rendu se crée sur elle.
    NkContextDesc desc;
    desc.api = NkGraphicsApi::NK_GFX_API_OPENGL;
    NkRenderWindow cible(fenetre, desc);
    if (!cible.IsValid()) {
        fenetre.Close();
        return -2;
    }

    NkRectangleShape carre({50.f, 50.f});
    carre.SetFillColor(NkColor2D{220, 40, 40, 255});
    float32 x = 0.f;
    bool enMarche = true;
    NkClock horloge;
    auto &evenements = NkEvents();

    while (enMarche && fenetre.IsOpen()) {
        // 1. Le temps.
        float32 dt = horloge.Tick().delta;
        if (dt > 0.1f) {
            dt = 0.1f;
        }
        // 2. Les événements.
        while (NkEvent *ev = evenements.PollEvent()) {
            if (ev->As<NkWindowCloseEvent>() != nullptr) {
                enMarche = false;
            }
        }
        // 3. La mise à jour.
        x += 100.f * dt;
        if (x > 800.f) {
            x = -50.f;
        }
        carre.SetPosition({x, 200.f});
        // 4. Clear, 5. le dessin, 6. Display.
        cible.Clear(NkColor2D{18, 18, 24, 255});
        cible.Draw(carre);
        cible.Display();
    }
    fenetre.Close();
    return 0;
}
