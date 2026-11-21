// Chapitre 4 — Exercice 4 : la coquille.
// Un carré rouge avance de 100 pixels par seconde sur un fond sombre.
#include "NKCanvas/App/NkCanvasApp.h"
#include "NKCanvas/Renderer/Shapes/NkRectangleShape.h"
#include "NKWindow/NKMain.h"

using namespace nkentseu;
using namespace nkentseu::renderer;

class Coquille : public NkCanvasApp {
    public:
        Coquille() {
            Config().title = "Coquille";
            Config().width = 800;
            Config().height = 450;
            Config().clearColor = NkColor2D{18, 18, 24, 255};
            mCarre.SetSize({50.f, 50.f});
            mCarre.SetFillColor(NkColor2D{220, 40, 40, 255});
        }

    protected:
        void OnUpdate(float32 dt) override {
            mX += 100.f * dt;
            if (mX > 800.f) {
                mX = -50.f;
            }
            mCarre.SetPosition({mX, 200.f});
        }

        void OnRender(NkRenderWindow &cible) override {
            cible.Draw(mCarre);
        }

    private:
        NkRectangleShape mCarre;
        float32 mX = 0.f;
};

int nkmain(const NkEntryState &state) {
    return NkCanvasApp::Run<Coquille>(state);
}
