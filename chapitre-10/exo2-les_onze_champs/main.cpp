// Chapitre 10 — Exercice 2 : les onze champs.
// Rejoue l'enregistrement puis la réouverture d'une planche : quels champs reviennent changés ?
#include <iostream>
#include <string>

int main() {
    int n = 0;
    std::cin >> n;
    int restent = 0;
    int changes = 0;
    for (int k = 1; k <= n; ++k) {
        // Les treize mots, lus tous, même ceux que le format conserve.
        long long id = 0;
        std::string x, y, echelle, rotation, opacite, miroirX, miroirY, largeur, hauteur;
        int selectionne = 0;
        std::string chemin;
        long long octets = 0;
        std::cin >> id >> x >> y >> echelle >> rotation >> opacite >> miroirX >> miroirY >> largeur >> hauteur
            >> selectionne >> chemin >> octets;
        if (octets == 0) {
            // Rien à décoder : l'image ne revient pas, les suivantes remontent.
            std::cout << "ITEM " << k << " SAUTE\n";
            continue;
        }
        ++restent;
        std::cout << "ITEM " << k << ' ' << restent;
        int ici = 0;
        // Le fichier n'écrit ni l'identifiant, ni la sélection, ni le chemin.
        if (id != 0) {
            std::cout << " id";
            ++ici;
        }
        if (selectionne != 0) {
            std::cout << " selected";
            ++ici;
        }
        if (chemin != "-") {
            std::cout << " sourcePath";
            ++ici;
        }
        if (ici == 0) {
            std::cout << " AUCUN";
        }
        std::cout << '\n';
        changes += ici;
    }
    std::cout << "RESTENT " << restent << '\n';
    std::cout << "CHANGES " << changes << '\n';
    return 0;
}
