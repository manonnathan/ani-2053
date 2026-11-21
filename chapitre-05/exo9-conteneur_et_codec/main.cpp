// Chapitre 5 — Exercice 9 : conteneur et codec.
// Le conteneur se reconnaît aux octets ; le codec se lit sur chaque piste d'un MP4.
#include <iostream>
#include <string>
#include <vector>

static int HexVal(char c) {
    if (c >= '0' && c <= '9') {
        return c - '0';
    }
    if (c >= 'a' && c <= 'f') {
        return c - 'a' + 10;
    }
    if (c >= 'A' && c <= 'F') {
        return c - 'A' + 10;
    }
    return 0;
}

static std::vector<unsigned int> Decode(const std::string &hex) {
    std::vector<unsigned int> b;
    if (hex == "-") {
        return b;
    }
    for (size_t i = 0; i + 1 < hex.size(); i += 2) {
        b.push_back(static_cast<unsigned int>(HexVal(hex[i]) * 16 + HexVal(hex[i + 1])));
    }
    return b;
}

static bool A(const std::vector<unsigned int> &b, size_t depuis, std::initializer_list<unsigned int> sig) {
    if (depuis + sig.size() > b.size()) {
        return false;
    }
    size_t i = depuis;
    for (unsigned int v : sig) {
        if (b[i] != v) {
            return false;
        }
        ++i;
    }
    return true;
}

static std::string Conteneur(const std::vector<unsigned int> &b) {
    size_t n = b.size();
    if (n >= 12 && A(b, 4, {0x66, 0x74, 0x79, 0x70})) {
        return "MP4";
    }
    if (n >= 4 && A(b, 0, {0x1A, 0x45, 0xDF, 0xA3})) {
        return "WEBM";
    }
    if (n >= 12 && A(b, 0, {0x52, 0x49, 0x46, 0x46}) && A(b, 8, {0x57, 0x41, 0x56, 0x45})) {
        return "WAV";
    }
    if (n >= 4 && A(b, 0, {0x4F, 0x67, 0x67, 0x53})) {
        return "OGG";
    }
    if (n >= 4 && A(b, 0, {0x66, 0x4C, 0x61, 0x43})) {
        return "FLAC";
    }
    if (n >= 3 && A(b, 0, {0x49, 0x44, 0x33})) {
        return "MP3";
    }
    if (n >= 2 && b[0] == 0xFF && b[1] >= 0xE0) {
        return "MP3";
    }
    return "INCONNU";
}

static std::string Codec(const std::string &code) {
    if (code == "mp4a") {
        return "aac";
    }
    if (code == "Opus" || code == "opus") {
        return "opus";
    }
    if (code == "avc1" || code == "avc3") {
        return "h264";
    }
    if (code == "hvc1" || code == "hev1") {
        return "h265";
    }
    if (code == "vp08") {
        return "vp8";
    }
    if (code == "vp09") {
        return "vp9";
    }
    if (code == "mp4v") {
        return "mpeg4";
    }
    if (code == ".mp3") {
        return "mp3";
    }
    if (code == "twos" || code == "sowt" || code == "lpcm") {
        return "pcm";
    }
    // Absent de la table : le code garde son nom, tel quel.
    return code;
}

static bool Lisible(const std::string &code) {
    return code == "mjpa" || code == "jpeg" || code == "MJPG" || code == "avc1" || code == "avc3" ||
           code == "hvc1" || code == "hev1" || code == "av01";
}

int main() {
    long long n = 0;
    if (!(std::cin >> n)) {
        n = 0;
    }
    long long mp4 = 0;
    long long lisibles = 0;
    long long inconnus = 0;

    for (long long i = 0; i < n; ++i) {
        std::string nom;
        std::string hex;
        long long p = 0;
        std::cin >> nom >> hex >> p;
        // On lit TOUTES les pistes, même si elles seront ignorées.
        std::vector<std::string> types;
        std::vector<std::string> codes;
        for (long long k = 0; k < p; ++k) {
            std::string type;
            std::string code;
            std::cin >> type >> code;
            types.push_back(type);
            codes.push_back(code);
        }

        std::string conteneur = Conteneur(Decode(hex));
        std::cout << nom << ' ' << conteneur << '\n';
        if (conteneur == "INCONNU") {
            ++inconnus;
            continue;
        }
        if (conteneur != "MP4") {
            continue;
        }
        ++mp4;
        std::string premiereImage;
        bool aImage = false;
        for (size_t k = 0; k < types.size(); ++k) {
            std::string genre = types[k];
            if (types[k] == "vide") {
                genre = "VIDEO";
                if (!aImage) {
                    aImage = true;
                    premiereImage = codes[k];
                }
            } else if (types[k] == "soun") {
                genre = "AUDIO";
            }
            std::cout << nom << " PISTE " << (k + 1) << ' ' << genre << ' ' << Codec(codes[k]) << '\n';
        }
        // Le lecteur ne juge que la première piste vide.
        if (!aImage) {
            std::cout << nom << " LECTEUR SANS_IMAGE\n";
        } else if (Lisible(premiereImage)) {
            std::cout << nom << " LECTEUR LISIBLE\n";
            ++lisibles;
        } else {
            std::cout << nom << " LECTEUR ECHEC\n";
        }
    }

    std::cout << "MP4 " << mp4 << '\n';
    std::cout << "LISIBLES " << lisibles << '\n';
    std::cout << "INCONNUS " << inconnus << '\n';
    return 0;
}
