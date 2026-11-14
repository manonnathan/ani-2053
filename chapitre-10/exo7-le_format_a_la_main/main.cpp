// Chapitre 10 — Exercice 7 : le format à la main.
// Décode octet par octet un fichier .nkref, sans ouvrir l'outil.
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <string>
#include <vector>

namespace {
    // Lecteur qui vérifie qu'il reste assez d'octets avant chaque lecture.
    struct Lecteur {
        const std::vector<uint8_t> &o;
        size_t pos = 0;
        bool tronque = false;

        explicit Lecteur(const std::vector<uint8_t> &octets) : o(octets) {}

        bool Reste(size_t n) {
            if (tronque || o.size() - pos < n) {
                tronque = true;
                return false;
            }
            return true;
        }
        uint8_t U8() {
            return Reste(1) ? o[pos++] : 0;
        }
        uint32_t U32() {
            if (!Reste(4)) {
                return 0;
            }
            // Petit-boutiste : le premier octet lu est le poids faible.
            uint32_t v = static_cast<uint32_t>(o[pos]) | (static_cast<uint32_t>(o[pos + 1]) << 8) |
                         (static_cast<uint32_t>(o[pos + 2]) << 16) | (static_cast<uint32_t>(o[pos + 3]) << 24);
            pos += 4;
            return v;
        }
        float F32() {
            uint32_t bits = U32();
            float f = 0.f;
            std::memcpy(&f, &bits, 4);
            return f;
        }
        void Sauter(uint32_t n) {
            if (Reste(n)) {
                pos += n;
            }
        }
    };

    std::string Nombre(double v) {
        char buf[64];
        std::snprintf(buf, sizeof(buf), "%.2f", v);
        std::string s = buf;
        if (!s.empty() && s[0] == '-' && s.find_first_not_of("0.", 1) == std::string::npos) {
            s.erase(0, 1);
        }
        return s;
    }

    int Hex(char c) {
        if (c >= '0' && c <= '9') {
            return c - '0';
        }
        if (c >= 'A' && c <= 'F') {
            return c - 'A' + 10;
        }
        if (c >= 'a' && c <= 'f') {
            return c - 'a' + 10;
        }
        return 0;
    }
}

int main() {
    long t = 0;
    std::cin >> t;
    std::vector<uint8_t> octets;
    std::string mot;
    for (long i = 0; i < t && std::cin >> mot; ++i) {
        octets.push_back(static_cast<uint8_t>(Hex(mot[0]) * 16 + Hex(mot.size() > 1 ? mot[1] : '0')));
    }

    Lecteur r(octets);
    uint8_t magie[4];
    for (uint8_t &m : magie) {
        m = r.U8();
    }
    if (r.tronque) {
        std::cout << "TRONQUE\n";
        return 0;
    }
    if (magie[0] != 'N' || magie[1] != 'K' || magie[2] != 'R' || magie[3] != 'F') {
        std::cout << "PAS_UNE_PLANCHE\n";
        return 0;
    }
    uint32_t version = r.U32();
    if (r.tronque) {
        std::cout << "TRONQUE\n";
        return 0;
    }
    if (version != 1) {
        std::cout << "VERSION_INCONNUE " << version << '\n';
        return 0;
    }

    // La suite s'écrit dans un tampon : un fichier tronqué n'affiche qu'une ligne.
    std::string sortie;
    float cx = r.F32();
    float cy = r.F32();
    float zoom = r.F32();
    uint8_t theme = r.U8();
    uint8_t grille = r.U8();
    r.U8();
    r.U8();
    sortie += "VUE " + Nombre(cx) + ' ' + Nombre(cy) + ' ' + Nombre(zoom) + '\n';
    sortie += std::string("THEME ") + (theme != 0 ? "SOMBRE" : "CLAIR") + '\n';
    sortie += std::string("GRILLE ") + (grille == 0 ? "AFFICHEE" : "CACHEE") + '\n';
    uint32_t images = r.U32();
    if (!r.tronque) {
        sortie += "IMAGES " + std::to_string(images) + '\n';
    }
    for (uint32_t k = 1; k <= images && !r.tronque; ++k) {
        float v[5];
        for (float &f : v) {
            f = r.F32();
        }
        uint8_t mx = r.U8();
        uint8_t my = r.U8();
        r.U8();
        r.U8();
        uint32_t largeur = r.U32();
        uint32_t hauteur = r.U32();
        uint32_t taille = r.U32();
        r.Sauter(taille);  // les octets source
        sortie += "IMAGE " + std::to_string(k);
        for (float f : v) {
            sortie += ' ' + Nombre(f);
        }
        sortie += ' ' + std::to_string(mx != 0 ? 1 : 0) + ' ' + std::to_string(my != 0 ? 1 : 0) + ' ' +
                  std::to_string(largeur) + ' ' + std::to_string(hauteur) + ' ' + std::to_string(taille) + '\n';
    }
    uint32_t traits = r.U32();
    if (!r.tronque) {
        sortie += "TRAITS " + std::to_string(traits) + '\n';
    }
    for (uint32_t k = 1; k <= traits && !r.tronque; ++k) {
        uint8_t c[4];
        for (uint8_t &x : c) {
            x = r.U8();
        }
        float epaisseur = r.F32();
        uint32_t p = r.U32();
        sortie += "TRAIT " + std::to_string(k) + ' ' + std::to_string(c[0]) + ' ' + std::to_string(c[1]) + ' ' +
                  std::to_string(c[2]) + ' ' + std::to_string(c[3]) + ' ' + Nombre(epaisseur) + ' ' +
                  std::to_string(p) + '\n';
        for (uint32_t i = 0; i < p && !r.tronque; ++i) {
            float x = r.F32();
            float y = r.F32();
            sortie += "POINT " + Nombre(x) + ' ' + Nombre(y) + '\n';
        }
    }
    if (r.tronque) {
        std::cout << "TRONQUE\n";
        return 0;
    }
    std::cout << sortie;
    return 0;
}
