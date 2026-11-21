// Chapitre 5 — Exercice 2 : l'extension qui ment.
// Reconnaît le format d'une image à ses premiers octets, jamais à son nom.
#include <cctype>
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

// Les octets sont des valeurs de 0 à 255 : jamais de char signé.
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

// Vrai si les octets donnés commencent par la signature, à partir de « depuis ».
static bool Commence(const std::vector<unsigned int> &b, size_t depuis, std::initializer_list<unsigned int> sig) {
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

static std::string Reconnait(long long taille, const std::vector<unsigned int> &b) {
    if (taille < 4) {
        return "";
    }
    if (taille >= 8 && Commence(b, 0, {0x89, 0x50, 0x4E, 0x47})) {
        return "PNG";
    }
    if (Commence(b, 0, {0xFF, 0xD8, 0xFF})) {
        return "JPEG";
    }
    if (Commence(b, 0, {0x42, 0x4D})) {
        return "BMP";
    }
    if (Commence(b, 0, {0x71, 0x6F, 0x69, 0x66})) {
        return "QOI";
    }
    if (Commence(b, 0, {0x47, 0x49, 0x46, 0x38})) {
        return "GIF";
    }
    if (b.size() >= 4 && b[0] == 0x00 && b[1] == 0x00 && (b[2] == 0x01 || b[2] == 0x02) && b[3] == 0x00) {
        return "ICO";
    }
    if (taille >= 10 && Commence(b, 0, {0x23, 0x3F})) {
        return "HDR";
    }
    if (Commence(b, 0, {0x76, 0x2F, 0x31, 0x01})) {
        return "EXR";
    }
    if (b.size() >= 2 && b[0] == 0x50 && b[1] >= 0x31 && b[1] <= 0x36) {
        if (b[1] == 0x31 || b[1] == 0x34) {
            return "PBM";
        }
        if (b[1] == 0x32 || b[1] == 0x35) {
            return "PGM";
        }
        return "PPM";
    }
    if (taille >= 18 && b.size() >= 3) {
        unsigned int t = b[2];
        if (t == 0x00 || t == 0x01 || t == 0x02 || t == 0x03 || t == 0x09 || t == 0x0A || t == 0x0B) {
            return "TGA";
        }
    }
    size_t i = 0;
    if (Commence(b, 0, {0xEF, 0xBB, 0xBF})) {
        i = 3;
    }
    while (i < b.size() && (b[i] == 0x20 || b[i] == 0x09 || b[i] == 0x0A || b[i] == 0x0D)) {
        ++i;
    }
    if (Commence(b, i, {0x3C, 0x3F, 0x78, 0x6D, 0x6C}) || Commence(b, i, {0x3C, 0x73, 0x76, 0x67})) {
        return "SVG";
    }
    return "";
}

static bool ExtensionJuste(const std::string &format, const std::string &ext) {
    if (ext.empty()) {
        return false;
    }
    if (format == "JPEG") {
        return ext == "jpg" || ext == "jpeg";
    }
    if (format == "ICO") {
        return ext == "ico" || ext == "cur";
    }
    std::string attendu;
    for (char c : format) {
        attendu.push_back(static_cast<char>(std::tolower(static_cast<unsigned char>(c))));
    }
    return ext == attendu;
}

int main() {
    long long n = 0;
    if (!(std::cin >> n)) {
        n = 0;
    }
    long long lus = 0;
    long long mensonges = 0;
    long long refuses = 0;

    for (long long k = 0; k < n; ++k) {
        std::string nom;
        long long taille = 0;
        std::string hex;
        std::cin >> nom >> taille >> hex;

        std::string format = Reconnait(taille, Decode(hex));
        if (format.empty()) {
            std::cout << nom << " REFUSE\n";
            ++refuses;
            continue;
        }
        // L'extension est après le DERNIER point, en minuscules.
        std::string ext;
        size_t point = nom.rfind('.');
        if (point != std::string::npos) {
            for (size_t i = point + 1; i < nom.size(); ++i) {
                ext.push_back(static_cast<char>(std::tolower(static_cast<unsigned char>(nom[i]))));
            }
        }
        ++lus;
        if (ExtensionJuste(format, ext)) {
            std::cout << nom << ' ' << format << " OK\n";
        } else {
            std::cout << nom << ' ' << format << " MENT\n";
            ++mensonges;
        }
    }

    std::cout << "LUS " << lus << '\n';
    std::cout << "MENSONGES " << mensonges << '\n';
    std::cout << "REFUSES " << refuses << '\n';
    return 0;
}
