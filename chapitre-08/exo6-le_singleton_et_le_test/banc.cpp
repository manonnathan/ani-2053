// Chapitre 8 — Exercice 6 : le singleton et le test.
// OnCommandLine fait tourner le banc AVANT toute fenêtre ; le code de sortie est le nombre d'échecs.
#include <cstdio>

#include "NKCanvas/App/NkCanvasApp.h"
#include "NKEvent/NkEventDispatcher.h"
#include "NKWindow/NKMain.h"

using namespace nkentseu;
using namespace nkentseu::renderer;

// ── Version directe : la règle lit le singleton. Pour la tester, il faudrait
//    qu'une vraie barre d'espace soit enfoncée au bon moment. ──────────────────
bool DoitSauterDirect(bool auSol) {
    return auSol && NkInput.IsKeyDown(NkKey::NK_SPACE);
}

// ── Version testable : la même règle, l'état de la touche arrive en paramètre. ──
bool DoitSauter(bool auSol, bool espaceEnfoncee) {
    return auSol && espaceEnfoncee;
}

// Le jeu, lui, garde l'appel au singleton en un seul endroit, à la frontière.
bool DoitSauterDansLeJeu(bool auSol) {
    return DoitSauter(auSol, NkInput.IsKeyDown(NkKey::NK_SPACE));
}

namespace {
#if defined(NDEBUG)
    const char *const kFichier = "banc-Release.txt";
#else
    const char *const kFichier = "banc-Debug.txt";
#endif

    struct Cas {
        bool auSol;
        bool espace;
        bool attendu;
    };
    const Cas kCas[] = {
        {true, true, true},    // au sol, espace enfoncée : il saute
        {true, false, false},  // au sol, espace relâchée : non
        {false, true, false},  // en l'air, espace enfoncée : pas de double saut
        {false, false, false}, // en l'air, rien : non
    };
}

class Banc : public NkCanvasApp {
    protected:
        NkOptional<int> OnCommandLine(const NkVector<NkString> &args) override {
            (void)args;
            std::FILE *f = std::fopen(kFichier, "w");
            int echecs = 0;
            int numero = 0;
            for (const Cas &c : kCas) {
                bool obtenu = DoitSauter(c.auSol, c.espace);
                bool ok = (obtenu == c.attendu);
                echecs += ok ? 0 : 1;
                if (f != nullptr) {
                    std::fprintf(f, "cas %d testable : auSol=%d espace=%d attendu=%d obtenu=%d %s\n", ++numero,
                                 c.auSol, c.espace, c.attendu, obtenu, ok ? "OK" : "ECHEC");
                }
            }
            // La version directe, au même endroit : on ne peut pas lui fournir la touche.
            numero = 0;
            for (const Cas &c : kCas) {
                bool obtenu = DoitSauterDirect(c.auSol);
                bool ok = (obtenu == c.attendu);
                if (f != nullptr) {
                    std::fprintf(f, "cas %d directe  : auSol=%d espace=%d attendu=%d obtenu=%d %s\n", ++numero,
                                 c.auSol, c.espace, c.attendu, obtenu, ok ? "OK" : "ECHEC");
                }
            }
            if (f != nullptr) {
                std::fprintf(f, "NkInput.IsKeyDown(NK_SPACE) avant toute fenetre : %d\n",
                             NkInput.IsKeyDown(NkKey::NK_SPACE) ? 1 : 0);
                std::fprintf(f, "echecs (version testable) : %d\n", echecs);
                std::fclose(f);
            }
            return NkOptional<int>(echecs);  // le code de sortie EST le verdict
        }
};

int nkmain(const NkEntryState &state) {
    return NkCanvasApp::Run<Banc>(state);
}
