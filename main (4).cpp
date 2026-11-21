// Chapitre 11 — Exercice 7 : le scénario reproductible.
// Découpe les variables NK_AGENT_* comme l'outil piloté, puis déroule les tours de sa boucle.
#include <algorithm>
#include <iostream>
#include <string>
#include <vector>

namespace {
    long gIgnores = 0;

    std::vector<std::string> Couper(const std::string &s, char sep) {
        std::vector<std::string> morceaux;
        std::string cur;
        for (char c : s) {
            if (c == sep) {
                morceaux.push_back(cur);
                cur.clear();
            } else {
                cur += c;
            }
        }
        morceaux.push_back(cur);
        return morceaux;
    }

    // Un entier : un signe moins facultatif, puis au moins un chiffre, et rien d'autre.
    bool Entier(const std::string &s, long long &v) {
        size_t i = (!s.empty() && s[0] == '-') ? 1 : 0;
        if (i >= s.size() || s.size() - i > 18) {
            return false;
        }
        for (size_t j = i; j < s.size(); ++j) {
            if (s[j] < '0' || s[j] > '9') {
                return false;
            }
        }
        v = std::stoll(s);
        return true;
    }

    // Ordre d'un même tour : PAN, ZOOM, DROP, CLICK, MOVE, PACK, GRID, puis les captures.
    enum Rang { PAN = 0, ZOOM, DROP, CLICK, MOVE, PACK, GRID, SHOT };

    struct Evenement {
        long long n = 0;      // numéro de trame écrit
        int rang = 0;
        long ordre = 0;       // ordre d'écriture dans sa liste
        std::string texte;    // ce qui s'affiche après « TOUR t »
    };

    std::vector<Evenement> gEv;
    bool gASortie = false;
    long long gSortie = 0;

    void Ajouter(long long n, int rang, long ordre, const std::string &texte) {
        gEv.push_back({n, rang, ordre, texte});
    }

    // Une valeur simple « n:a:b... » : au moins « champs » champs, n entier.
    bool Simple(const std::string &v, size_t champs, std::vector<std::string> &f, long long &n) {
        f = Couper(v, ':');
        if (f.size() < champs || !Entier(f[0], n)) {
            ++gIgnores;
            return false;
        }
        return true;
    }
}

int main() {
    std::string ligne;
    while (std::getline(std::cin, ligne)) {
        while (!ligne.empty() && (ligne.back() == '\r' || ligne.back() == ' ' || ligne.back() == '\t')) {
            ligne.pop_back();
        }
        const size_t eq = ligne.find('=');
        if (eq == std::string::npos) {
            continue;
        }
        const std::string nom = ligne.substr(0, eq);
        const std::string v = ligne.substr(eq + 1);
        std::vector<std::string> f;
        long long n = 0;
        if (nom == "NK_AGENT_SHOT") {
            long gardes = 0;
            for (const std::string &e : Couper(v, ',')) {
                if (gardes < 8 && Entier(e, n)) {
                    Ajouter(n, SHOT, gardes++, "CAPTURE");
                } else {
                    ++gIgnores;  // invalide, ou au-delà des 8 premiers valides
                }
            }
        } else if (nom == "NK_AGENT_EXIT") {
            if (Entier(v, n)) {
                gASortie = true;
                gSortie = n;
            } else {
                ++gIgnores;
            }
        } else if (nom == "NK_AGENT_PAN") {
            if (Simple(v, 3, f, n)) {
                Ajouter(n, PAN, 0, "PAN " + f[1] + " " + f[2]);
            }
        } else if (nom == "NK_AGENT_ZOOM") {
            if (Simple(v, 4, f, n)) {
                Ajouter(n, ZOOM, 0, "ZOOM " + f[1] + " " + f[2] + " " + f[3]);
            }
        } else if (nom == "NK_AGENT_MOVE") {
            if (Simple(v, 3, f, n)) {
                Ajouter(n, MOVE, 0, "MOVE " + f[1] + " " + f[2]);
            }
        } else if (nom == "NK_AGENT_PACK") {
            if (Simple(v, 1, f, n)) {
                Ajouter(n, PACK, 0, "RANGE");
            }
        } else if (nom == "NK_AGENT_GRID") {
            if (Simple(v, 2, f, n)) {
                Ajouter(n, GRID, 0, "GRILLE " + f[1]);
            }
        } else if (nom == "NK_AGENT_CLICK") {
            long gardes = 0;
            for (const std::string &e : Couper(v, ';')) {
                f = Couper(e, ':');
                if (gardes < 8 && f.size() >= 3 && Entier(f[0], n)) {
                    Ajouter(n, CLICK, gardes++, "CLIC " + f[1] + " " + f[2]);
                } else {
                    ++gIgnores;
                }
            }
        } else if (nom == "NK_AGENT_DROP") {
            long gardes = 0;
            for (const std::string &e : Couper(v, ';')) {
                // Le chemin est tout ce qui suit le TROISIÈME deux-points : il peut en contenir.
                const size_t c1 = e.find(':');
                const size_t c2 = c1 == std::string::npos ? c1 : e.find(':', c1 + 1);
                const size_t c3 = c2 == std::string::npos ? c2 : e.find(':', c2 + 1);
                if (gardes < 8 && c3 != std::string::npos && Entier(e.substr(0, c1), n)) {
                    const std::string px = e.substr(c1 + 1, c2 - c1 - 1);
                    const std::string py = e.substr(c2 + 1, c3 - c2 - 1);
                    Ajouter(n, DROP, gardes++, "DEPOT " + e.substr(c3 + 1) + " " + px + " " + py);
                } else {
                    ++gIgnores;
                }
            }
        }
    }

    const bool arret = gASortie && gSortie > 0;  // une sortie nulle ou négative n'arrête jamais
    // Tour où chaque événement a lieu : une action au tour n, une capture au tour n - 1
    // (le compteur vient de passer à n). Un tour au-delà de sortie - 1 n'est jamais joué.
    std::vector<std::pair<long long, size_t>> aLieu;
    long jamais = 0;
    for (size_t i = 0; i < gEv.size(); ++i) {
        const Evenement &e = gEv[i];
        const long long tour = e.rang == SHOT ? e.n - 1 : e.n;
        const bool part = e.rang == SHOT ? e.n >= 1 : e.n > 0;
        if (!part || (arret && tour > gSortie - 1)) {
            ++jamais;
            continue;
        }
        aLieu.push_back({tour, i});
    }
    std::stable_sort(aLieu.begin(), aLieu.end(), [](const auto &x, const auto &y) {
        const Evenement &a = gEv[x.second];
        const Evenement &b = gEv[y.second];
        if (x.first != y.first) {
            return x.first < y.first;
        }
        if (a.rang != b.rang) {
            return a.rang < b.rang;
        }
        return a.ordre < b.ordre;
    });
    for (const auto &p : aLieu) {
        std::cout << "TOUR " << p.first << ' ' << gEv[p.second].texte << '\n';
    }
    if (arret) {
        std::cout << "ARRET " << (gSortie - 1) << '\n';
    } else {
        std::cout << "SANS_ARRET\n";
    }
    std::cout << "JAMAIS " << jamais << '\n';
    std::cout << "IGNORES " << gIgnores << '\n';
    return 0;
}
