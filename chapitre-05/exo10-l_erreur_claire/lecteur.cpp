// Chapitre 5 — Exercice 10 : l'erreur claire.
// Usage : lecteur film.mp4 film-coupe.mp4 texte.mp4
// Open ne rend que vrai ou faux : la sonde dit ce que le fichier contient vraiment.
#include <cstdio>
#include <new>

#include "NKLogger/NkLog.h"
#include "NKMedia/NkMediaProbe.h"
#include "NKMedia/Video/NkVideoReader.h"

using namespace nkentseu;
using namespace nkentseu::media;

static const char *Genre(NkMediaTrackType t) {
    if (t == NkMediaTrackType::NK_VIDEO) {
        return "video";
    }
    if (t == NkMediaTrackType::NK_AUDIO) {
        return "audio";
    }
    return "inconnu";
}

int main(int argc, char **argv) {
    for (int i = 1; i < argc; ++i) {
        const char *chemin = argv[i];

        NkVideoReader lecteur;
        bool ouvert = lecteur.Open(chemin);
        int images = 0;
        // On teste le retour de Open AVANT de lire quoi que ce soit.
        if (ouvert) {
            const NkVideoReaderInfo &info = lecteur.Info();
            logger.Info("[lecteur] {0} : conteneur {1}, codec {2}, {3} x {4}", chemin, info.container.CStr(),
                        info.codec.CStr(), info.width, info.height);
            // Un fichier coupé peut s'ouvrir et échouer plus loin : on compte les images.
            NkVideoFrame image;
            while (lecteur.ReadFrame(image)) {
                ++images;
            }
        } else {
            logger.Warn("[lecteur] {0} : Open a refuse", chemin);
        }

        NkMediaInfo sonde;
        bool sondeOk = NkMediaProbe::ProbeFile(chemin, sonde);
        logger.Info("[sonde] {0} : {1}, {2} piste(s)", chemin, sondeOk ? sonde.ContainerName() : "echec",
                    static_cast<int>(sonde.tracks.Size()));
        for (const NkMediaTrack &t : sonde.tracks) {
            logger.Info("[sonde]   piste {0} : codec {1}, {2} x {3}", Genre(t.type), t.codec.CStr(), t.width,
                        t.height);
        }

        std::printf("fichier : %s, ouvert : %s, images : %d, sonde : %s\n", chemin, ouvert ? "oui" : "non", images,
                    sondeOk ? sonde.ContainerName() : "ECHEC");
    }
    return 0;
}
