// Chapitre 4 — Exercice 6 : événement ou interrogation.
// Rejoue les mêmes touches par événements et par interrogation d'état.
#include <iostream>
#include <string>

int main() {
    long long v = 0;
    long long n = 0;
    if (!(std::cin >> v >> n)) {
        v = 0;
        n = 0;
    }
    bool space = false;
    bool left = false;
    bool right = false;
    long long xe = 0;
    long long xi = 0;
    long long sautsEvenements = 0;
    long long sautsInterrogation = 0;
    long long manques = 0;

    for (long long i = 1; i <= n; ++i) {
        long long k = 0;
        std::cin >> k;
        long long appuisSpaceImage = 0;

        for (long long e = 0; e < k; ++e) {
            std::string ev;
            std::cin >> ev;
            if (ev.size() < 2) {
                continue;
            }
            bool enfonce = (ev[0] == '+');
            std::string nom = ev.substr(1);

            if (nom == "SPACE") {
                space = enfonce;
                if (enfonce) {
                    // Un + répété par le système compte un saut de plus.
                    ++sautsEvenements;
                    ++appuisSpaceImage;
                }
            } else if (nom == "RIGHT") {
                right = enfonce;
                if (enfonce) {
                    xe += v;
                }
            } else if (nom == "LEFT") {
                left = enfonce;
                if (enfonce) {
                    xe -= v;
                }
            }
            // Tout autre nom est ignoré.
        }

        // Interrogation : une seule fois par image, après tous les événements.
        if (space) {
            ++sautsInterrogation;
        }
        if (right) {
            xi += v;
        }
        if (left) {
            xi -= v;
        }
        if (!space) {
            manques += appuisSpaceImage;
        }

        std::cout << i << ' ' << xe << ' ' << xi << '\n';
    }

    std::cout << "SAUTS EVENEMENTS " << sautsEvenements << '\n';
    std::cout << "SAUTS INTERROGATION " << sautsInterrogation << '\n';
    std::cout << "MANQUES " << manques << '\n';
    return 0;
}
