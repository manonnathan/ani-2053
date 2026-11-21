// MonEssai — le plus petit projet du dépôt : il n'affiche rien.
// Il appelle une fonction de NKMath pour que l'édition de liens ait quelque chose à résoudre.
#include "NKMath/NkFunctions.h"

int main() {
    volatile float x = nkentseu::math::NkSqrt(2.f);
    (void)x;
    return 0;
}
