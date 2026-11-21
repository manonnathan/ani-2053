// Chapitre 4 — Exercice 10 : l'interface qui ne défile pas.
// Un monde plus large que la fenêtre défile sous une barre d'interface fixe.
// Argument « sans » : lance la version fautive (sans ResetView) pour la capture sans.png.
#include <cstdio>
#include <cstring>

#include "NKCanvas/App/NkCanvasApp.h"
#include "NKCanvas/Renderer/Resources/NkFont.h"
#include "NKCanvas/Renderer/Resources/NkSprite.h"  // NkText est déclaré ici
#include "NKCanvas/Renderer/Shapes/NkRectangleShape.h"
#include "NKWindow/NKMain.h"

using namespace nkentseu;
using namespace nkentseu::renderer;

namespace {
    bool gAvecReset = true;
}

class Interface : public NkCanvasApp {
    public:
        Interface() {
            Config().title = "Interface fixe";
            Config().width = 800;
            Config().height = 450;
            Config().clearColor = NkColor2D{18, 18, 24, 255};
        }

    protected:
        bool OnInit() override {
            mPolice.LoadFromFile(*Target().GetRenderer(), "Resources/Fonts/Cousine-Regular.ttf");
            mBarre.SetSize({800.f, 40.f});
            mBarre.SetPosition({0.f, 0.f});
            mBarre.SetFillColor(NkColor2D{40, 90, 160, 255});
            return true;
        }

        void OnUpdate(float32 dt) override {
            // La vue avance avec le temps écoulé, pas avec un compteur d'images.
            mCentreX += 120.f * dt;
            mJournal += dt;
        }

        void OnRender(NkRenderWindow &cible) override {
            NkView2D vue = cible.GetDefaultView();
            vue.center.x = mCentreX;
            cible.SetView(vue);

            // Le monde : une rangée de carrés, quatre fois la largeur de la fenêtre.
            NkRectangleShape carre({60.f, 60.f});
            for (int i = 0; i < 40; ++i) {
                carre.SetPosition({static_cast<float32>(i) * 80.f, 200.f});
                carre.SetFillColor((i % 2 == 0) ? NkColor2D{200, 200, 200, 255} : NkColor2D{230, 120, 40, 255});
                cible.Draw(carre);
            }

            // L'appel qui compte : la vue est un ÉTAT du renderer, elle s'appliquerait à la barre.
            if (gAvecReset) {
                cible.ResetView();
            }
            // Version fautive, gardée pour mémoire : la même fonction sans la ligne ci-dessus.
            //     cible.Draw(mBarre);   // dessinée dans la vue du monde : elle part avec lui

            cible.Draw(mBarre);
            // La position d'un texte est sa ligne de base : y = 28 le garde dans la barre.
            NkText texte(mPolice, "Score : 0   Vies : 3", 20);
            texte.SetFillColor(NkColor2D{255, 255, 255, 255});
            texte.SetPosition({12.f, 28.f});
            cible.Draw(texte);

            // barre_x : où la barre tombe à l'écran avec la vue réellement en place.
            NkView2D active = cible.GetView();
            float32 barreX = 0.f - (active.center.x - active.size.x * 0.5f);
            if (mJournal >= 0.5f) {
                mJournal = 0.f;
                std::printf("reset : %s, centre_x : %.0f, barre_x : %.0f\n", gAvecReset ? "oui" : "non", mCentreX,
                            barreX);
                std::fflush(stdout);
            }
        }

    private:
        // Deux NkFont existent (NKFont et NKCanvas) : on nomme celle de NKCanvas.
        renderer::NkFont mPolice;
        NkRectangleShape mBarre;
        float32 mCentreX = 400.f;
        float32 mJournal = 0.f;
};

int nkmain(const NkEntryState &state) {
    for (const NkString &arg : state.args) {
        if (arg == "sans") {
            gAvecReset = false;
        }
    }
    return NkCanvasApp::Run<Interface>(state);
}
