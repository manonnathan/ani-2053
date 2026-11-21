// Chapitre 4 — Exercice 1 : la fenêtre nue.
// Le plus petit programme NKCanvas : une fenêtre, une couleur, une fermeture propre.
#include "NKCanvas/App/NkCanvasApp.h"
#include "NKWindow/NKMain.h"

using namespace nkentseu;
using namespace nkentseu::renderer;

class Fenetre : public NkCanvasApp {
    public:
        Fenetre() {
            // Seul moment où la fenêtre n'existe pas encore : la configuration se pose ici.
            Config().title = "La fenetre nue";
            Config().width = 800;
            Config().height = 450;
            Config().clearColor = NkColor2D{18, 18, 24, 255};
        }
};

int nkmain(const NkEntryState &state) {
    return NkCanvasApp::Run<Fenetre>(state);
}
