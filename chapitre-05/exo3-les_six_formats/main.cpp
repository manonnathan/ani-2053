// Chapitre 5 — Exercice 3 : les six formats.
// Mémoire d'une image dans deux formats, et ce qu'une conversion lui fait perdre.
#include <iostream>
#include <string>

struct Format {
    const char *nom;
    long long octets;
    bool couleur;
    bool transparence;
    bool flottants;
};

static const Format kFormats[6] = {
    {"GRAY8", 1, false, false, false},
    {"GRAY_A16", 2, false, true, false},
    {"RGB24", 3, true, false, false},
    {"RGBA32", 4, true, true, false},
    {"RGB96F", 12, true, false, true},
    {"RGBA128F", 16, true, true, true},
};

static const Format *Cherche(const std::string &nom) {
    for (const Format &f : kFormats) {
        if (nom == f.nom) {
            return &f;
        }
    }
    return nullptr;
}

int main() {
    long long w = 1;
    long long h = 1;
    long long n = 0;
    std::cin >> w >> h >> n;
    // 64 bits : 65536 x 65536 en RGBA128F dépasse soixante milliards d'octets.
    long long pixels = w * h;
    long long total = 0;
    long long sansPerte = 0;
    long long refuses = 0;

    for (long long i = 0; i < n; ++i) {
        std::string source;
        std::string cible;
        std::cin >> source >> cible;
        const Format *fs = Cherche(source);
        const Format *fc = Cherche(cible);
        if (fs == nullptr || fc == nullptr) {
            std::cout << source << ' ' << cible << " REFUSE\n";
            ++refuses;
            continue;
        }
        std::string pertes;
        if (fs->transparence && !fc->transparence) {
            pertes += "TRANSPARENCE";
        }
        if (fs->couleur && !fc->couleur) {
            if (!pertes.empty()) {
                pertes += "+";
            }
            pertes += "COULEUR";
        }
        if (fs->flottants && !fc->flottants) {
            if (!pertes.empty()) {
                pertes += "+";
            }
            pertes += "ETENDUE";
        }
        if (pertes.empty()) {
            pertes = "AUCUNE";
            ++sansPerte;
        }
        long long octetsSource = pixels * fs->octets;
        long long octetsCible = pixels * fc->octets;
        total += octetsCible;
        std::cout << source << ' ' << cible << ' ' << octetsSource << ' ' << octetsCible << ' ' << pertes << '\n';
    }

    std::cout << "TOTAL " << total << '\n';
    std::cout << "SANS_PERTE " << sansPerte << '\n';
    std::cout << "REFUSES " << refuses << '\n';
    return 0;
}
