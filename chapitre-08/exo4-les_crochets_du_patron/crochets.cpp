// Chapitre 8 — Exercice 4 : les crochets du patron.
// Redéfinit chacun des douze crochets virtuels de NkCanvasApp ; chacun écrit une ligne :
// numéro d'ordre, nom, image en cours, Time(). Le programme s'arrête seul par Quit().
#include <cstdio>

#include "NKCanvas/App/NkCanvasApp.h"
#include "NKEvent/NkEvent.h"
#include "NKWindow/NKMain.h"

using namespace nkentseu;
using namespace nkentseu::renderer;

namespace {
#if defined(NDEBUG)
    const char *const kFichier = "crochets-Release.txt";
#else
    const char *const kFichier = "crochets-Debug.txt";
#endif
    std::FILE *gJournal = nullptr;
    int gOrdre = 0;
    constexpr int kImagesJournalisees = 3;  // OnUpdate/OnRender : les premières images seulement
    constexpr int kImagesAvantQuit = 5;
    constexpr int kEvenementsJournalises = 12;
}

class MonJeu : public NkCanvasApp {
    public:
        MonJeu() {
            Config().title = "Crochets";
            gJournal = std::fopen(kFichier, "w");
        }
        ~MonJeu() override {
            Note("~MonJeu (destructeur, pas un crochet)");
            if (gJournal != nullptr) {
                std::fclose(gJournal);
            }
        }

    protected:
        NkOptional<int> OnCommandLine(const NkVector<NkString> &args) override {
            Note("OnCommandLine", static_cast<int>(args.Size()));
            return NkOptional<int>();  // on continue : la fenêtre sera créée
        }
        bool OnInit() override {
            Note("OnInit");
            return true;
        }
        void OnLayout(const NkLayoutInfo &layout) override {
            char d[64];
            std::snprintf(d, sizeof(d), "%ux%u", layout.width, layout.height);
            Note("OnLayout", -1, d);
        }
        void OnUpdate(float32 dt) override {
            if (mImage < kImagesJournalisees) {
                char d[64];
                std::snprintf(d, sizeof(d), "dt=%.4f", dt);
                Note("OnUpdate", -1, d);
            }
        }
        void OnRender(NkRenderWindow &target) override {
            (void)target;
            if (mImage < kImagesJournalisees) {
                Note("OnRender");
            }
            ++mImage;  // l'image en cours se termine avec OnRender
            if (mImage == kImagesAvantQuit) {
                Note("Quit() demande");
                Quit();
            }
        }
        void OnShutdown() override {
            Note("OnShutdown");
        }
        bool OnEvent(const NkEvent &event) override {
            if (mEvenements < kEvenementsJournalises) {
                ++mEvenements;
                Note("OnEvent", -1, event.GetName());
            }
            return false;  // false : la coquille continue son travail typé
        }
        bool OnPointer(const NkPointer &pointer) override {
            (void)pointer;
            Note("OnPointer");
            return false;
        }
        bool OnKeyPress(const NkKeyPressEvent &event) override {
            (void)event;
            Note("OnKeyPress");
            return false;
        }
        bool OnCloseRequested() override {
            Note("OnCloseRequested");
            return true;
        }
        void OnPause() override {
            Note("OnPause");
        }
        void OnResume() override {
            Note("OnResume");
        }

    private:
        void Note(const char *crochet, int valeur = -1, const char *detail = nullptr) {
            if (gJournal == nullptr) {
                return;
            }
            std::fprintf(gJournal, "%3d  %-16s image %d  Time() %.4f", ++gOrdre, crochet, mImage, Time());
            if (valeur >= 0) {
                std::fprintf(gJournal, "  (%d arguments)", valeur);
            }
            if (detail != nullptr) {
                std::fprintf(gJournal, "  %s", detail);
            }
            std::fprintf(gJournal, "\n");
            std::fflush(gJournal);
        }

        int mImage = 0;
        int mEvenements = 0;
};

int nkmain(const NkEntryState &state) {
    return renderer::NkCanvasApp::Run<MonJeu>(state);
}
