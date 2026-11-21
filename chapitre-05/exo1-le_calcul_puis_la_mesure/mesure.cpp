// Chapitre 5 — Exercice 1 : le calcul, puis la mesure.
// Usage : mesure image1 image2 image3 image4 image5
// Pour chaque image : largeur, hauteur, BytesPP, calcul w*h*BytesPP, mémoire du
// processus juste avant et juste après le chargement (l'image reste en vie pendant
// la mesure), et taille du fichier.
#include <cstdio>
#include <new>

#if defined(_WIN32)
    #define PSAPI_VERSION 2  // GetProcessMemoryInfo -> K32GetProcessMemoryInfo, dans kernel32
    #include <windows.h>
    #include <psapi.h>
#endif

#include "NKImage/Core/NkImage.h"
#include "NKLogger/NkLog.h"

using namespace nkentseu;

// Mémoire résidente du processus, en octets : ce que montre le gestionnaire des tâches.
static long long MemoireProcessus() {
#if defined(_WIN32)
    PROCESS_MEMORY_COUNTERS pmc;
    if (GetProcessMemoryInfo(GetCurrentProcess(), &pmc, sizeof(pmc))) {
        return static_cast<long long>(pmc.WorkingSetSize);
    }
    return -1;
#else
    long long kio = -1;
    FILE *f = std::fopen("/proc/self/status", "r");
    if (f != nullptr) {
        char ligne[256];
        while (std::fgets(ligne, sizeof(ligne), f) != nullptr) {
            if (std::sscanf(ligne, "VmRSS: %lld kB", &kio) == 1) {
                break;
            }
        }
        std::fclose(f);
    }
    return kio < 0 ? -1 : kio * 1024;
#endif
}

static long long TailleFichier(const char *chemin) {
    FILE *f = std::fopen(chemin, "rb");
    if (f == nullptr) {
        return -1;
    }
    std::fseek(f, 0, SEEK_END);
    long long t = std::ftell(f);
    std::fclose(f);
    return t;
}

int main(int argc, char **argv) {
    // Les images restent en vie jusqu'à la fin : une image qui sort de sa portée libère ses pixels.
    static NkImage images[5];
    for (int i = 1; i < argc && i <= 5; ++i) {
        const char *chemin = argv[i];
        long long avant = MemoireProcessus();
        if (!images[i - 1].Load(chemin)) {
            logger.Error("[mesure] echec de chargement : {0}", chemin);
            continue;
        }
        long long apres = MemoireProcessus();
        const NkImage &img = images[i - 1];
        long long calcul = static_cast<long long>(img.Width()) * img.Height() * img.BytesPP();
        logger.Info("[mesure] {0} : {1} x {2}, BytesPP {3}", chemin, img.Width(), img.Height(), img.BytesPP());
        std::printf("image : %s, calcul : %lld, mesure : %lld, fichier : %lld\n", chemin, calcul, apres - avant,
                    TailleFichier(chemin));
    }
    return 0;
}
