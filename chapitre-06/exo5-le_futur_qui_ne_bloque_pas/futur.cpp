// Chapitre 6 — Exercice 5 : le futur qui ne bloque pas.
// Un carré avance d'un pixel par trame. À la trame 60, une grande image se charge dans
// une NkThreadPool. Deux façons d'en recevoir le résultat, choisies par kVersion :
//   VERSION_GET      futur.Get() juste après le lancement : la boucle attend.
//   VERSION_ISREADY  futur.IsReady() à chaque trame, Get() seulement quand c'est prêt.
// Usage : futur [chemin-de-la-grande-photo]
#include <cstdio>
#include <new>

#include "NKCanvas/App/NkCanvasApp.h"
#include "NKCanvas/Renderer/Shapes/NkRectangleShape.h"
#include "NKImage/Core/NkImage.h"
#include "NKThreading/NkFuture.h"
#include "NKThreading/NkPromise.h"
#include "NKThreading/NkThreadPool.h"
#include "NKTime/NkChrono.h"
#include "NKWindow/NKMain.h"

using namespace nkentseu;
using namespace nkentseu::renderer;
// NkFuture et NkPromise : alias de nkentseu, visibles par « using namespace nkentseu ».
using nkentseu::threading::NkThreadPool;

enum Version { VERSION_GET, VERSION_ISREADY };
// Changer ici pour passer d'une version à l'autre.
constexpr Version kVersion = VERSION_ISREADY;

namespace {
    const char *gChemin = "grande-photo.jpg";
}

class Futur : public NkCanvasApp {
    public:
        Futur() {
            Config().title = "Futur";
            Config().width = 800;
            Config().height = 450;
            Config().clearColor = NkColor2D{18, 18, 24, 255};
            mCarre.SetSize({40.f, 40.f});
            mCarre.SetFillColor(NkColor2D{220, 40, 40, 255});
        }

    protected:
        void OnUpdate(float32 dt) override {
            (void)dt;
            // La durée de la trame précédente, mesurée d'une trame à l'autre.
            double duree = mTrame.Reset().milliseconds;
            ++mNumero;
            if (mEnCours) {
                ++mTramesPendant;
                if (duree > mPlusLongue) {
                    mPlusLongue = duree;
                }
            }
            // La trame qui a reçu l'image (et, en version Get, l'a attendue) se mesure ici.
            if (mFiniALaProchaine) {
                mFiniALaProchaine = false;
                mEnCours = false;
                Bilan();
            }

            if (mNumero == 60) {
                mEnCours = true;
                mChargement.Reset();
                mFutur = mPromesse.GetFuture();
                // La promesse est un membre : elle vit jusqu'à ce que la tâche l'ait remplie.
                mPool.Enqueue([this]() {
                    NkImage img;
                    img.Load(gChemin);
                    // Une NkImage ne se copie pas : elle se déplace.
                    mPromesse.SetValue(traits::NkMove(img));
                });
                if (kVersion == VERSION_GET) {
                    // Bloque la boucle jusqu'à l'arrivée de l'image.
                    Recevoir();
                }
            }
            if (kVersion == VERSION_ISREADY && mEnCours && !mRecu && mFutur.IsReady()) {
                Recevoir();
            }

            // Un pixel par trame : le moindre gel se voit.
            mX += 1.f;
            if (mX > 800.f) {
                mX = -40.f;
            }
            mCarre.SetPosition({mX, 200.f});
        }

        void OnRender(NkRenderWindow &cible) override {
            // Seule la boucle dessine ; la tâche ne fait que charger des pixels.
            cible.Draw(mCarre);
        }

    private:
        void Recevoir() {
            const NkImage &img = mFutur.Get();
            double ms = mChargement.Elapsed().milliseconds;
            mRecu = true;
            mFiniALaProchaine = true;
            std::printf("Image : %d x %d, BytesPP %d, chargee en %.1f ms\n", img.Width(), img.Height(), img.BytesPP(),
                        ms);
        }

        void Bilan() {
            std::printf("%s : trame la plus longue %.1f ms, %d trame(s) pendant le chargement\n",
                        kVersion == VERSION_GET ? "Get" : "IsReady", mPlusLongue, mTramesPendant);
            std::fflush(stdout);
        }

    private:
        NkRectangleShape mCarre;
        float32 mX = 0.f;
        NkThreadPool mPool;
        NkPromise<NkImage> mPromesse;
        NkFuture<NkImage> mFutur;
        NkChrono mTrame;
        NkChrono mChargement;
        int mNumero = 0;
        bool mEnCours = false;
        bool mRecu = false;
        bool mFiniALaProchaine = false;
        int mTramesPendant = 0;
        double mPlusLongue = 0.0;
};

int nkmain(const NkEntryState &state) {
    if (state.args.Size() > 1) {
        gChemin = state.args[1].CStr();
    }
    return NkCanvasApp::Run<Futur>(state);
}
