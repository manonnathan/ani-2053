// Chapitre 8 — Exercice 8 : le composite à trois niveaux.
// Parent (200,150) -> parent (100,0), tourné de 90 degrés -> NkSprite (50,0).
// Une sonde placée comme le sprite écrit où son origine arrive une fois composée ;
// une seconde sonde imite l'ancien défaut : elle jette l'état reçu.
#include <cstdio>

#include "NKCanvas/App/NkCanvasApp.h"
#include "NKCanvas/Renderer/Core/NkDrawable.h"
#include "NKCanvas/Renderer/Core/NkRenderStates.h"
#include "NKCanvas/Renderer/Core/NkTransformable.h"
#include "NKCanvas/Renderer/Resources/NkSprite.h"
#include "NKCanvas/Renderer/Resources/NkTexture.h"
#include "NKWindow/NKMain.h"

using namespace nkentseu;
using namespace nkentseu::renderer;

namespace {
#if defined(NDEBUG)
    const char *const kFichier = "composite-Release.txt";
#else
    const char *const kFichier = "composite-Debug.txt";
#endif
}

// Un nœud : il compose son transform avec celui qu'il reçoit, puis dessine ses enfants.
// Les enfants sont tenus par pointeur : une référence construite depuis un autre nœud
// appellerait le constructeur de copie.
class Noeud : public NkTransformable, public NkDrawable {
    public:
        void Ajouter(const NkDrawable *enfant) {
            if (mNombre < 4) {
                mEnfants[mNombre++] = enfant;
            }
        }
        void Draw(NkRenderTarget &target, const NkRenderStates &states) const override {
            NkRenderStates local = states;
            local.transform *= GetTransform();  // l'état composé
            for (int i = 0; i < mNombre; ++i) {
                mEnfants[i]->Draw(target, local);
            }
        }

    private:
        const NkDrawable *mEnfants[4] = {nullptr, nullptr, nullptr, nullptr};
        int mNombre = 0;
};

// La sonde : placée comme le sprite, elle note où son origine (0,0) arrive.
class Sonde : public NkTransformable, public NkDrawable {
    public:
        explicit Sonde(bool jetteLEtat) : mJette(jetteLEtat) {}
        void Draw(NkRenderTarget &target, const NkRenderStates &states) const override {
            (void)target;
            NkTransform t;
            if (mJette) {
                t = GetTransform();  // l'ancien défaut : l'état reçu est ignoré
            } else {
                NkRenderStates local = states;
                local.transform *= GetTransform();
                t = local.transform;
            }
            mArrivee = t.TransformPoint({0.f, 0.f});
        }
        mutable NkVec2f mArrivee{0.f, 0.f};

    private:
        bool mJette;
};

class Composite : public NkCanvasApp {
    public:
        Composite() : mSonde(false), mSondeQuiJette(true) {
            Config().title = "Composite a trois niveaux";
            Config().width = 640;
            Config().height = 400;
        }

    protected:
        bool OnInit() override {
            if (!mTexture.Create(*Target().GetRenderer(), 32, 32, NkColor2D{255, 200, 0, 255})) {
                return false;
            }
            mSprite.SetTexture(mTexture, true);
            mSprite.SetPosition(50.f, 0.f);
            mSonde.SetPosition(50.f, 0.f);
            mSondeQuiJette.SetPosition(50.f, 0.f);

            mMilieu.SetPosition(100.f, 0.f);
            mMilieu.SetRotation(math::NkAngle(90.f));
            mMilieu.Ajouter(&mSprite);
            mMilieu.Ajouter(&mSonde);
            mMilieu.Ajouter(&mSondeQuiJette);

            mRacine.SetPosition(200.f, 150.f);
            mRacine.Ajouter(&mMilieu);
            return true;
        }

        void OnRender(NkRenderWindow &target) override {
            target.Draw(mRacine);
            ++mImage;
            if (mImage == 12) {
                std::FILE *f = std::fopen(kFichier, "w");
                if (f != nullptr) {
                    std::fprintf(f, "attendu        : (300.00, 200.00)\n");
                    std::fprintf(f, "sonde composee : (%.2f, %.2f)\n", mSonde.mArrivee.x, mSonde.mArrivee.y);
                    std::fprintf(f, "sonde qui jette l'etat : (%.2f, %.2f)\n", mSondeQuiJette.mArrivee.x,
                                 mSondeQuiJette.mArrivee.y);
                    std::fclose(f);
                }
                Quit();
            }
        }

    private:
        NkTexture mTexture;
        NkSprite mSprite;
        Sonde mSonde;
        Sonde mSondeQuiJette;
        Noeud mMilieu;
        Noeud mRacine;
        int mImage = 0;
};

int nkmain(const NkEntryState &state) {
    return NkCanvasApp::Run<Composite>(state);
}
