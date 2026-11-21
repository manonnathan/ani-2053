// Chapitre 9 — Exercice 10 : deux fois le même nom.
// Le dépôt définit deux NkWorld : collision::NkWorld (NKCollision, le monde des corps et des
// contacts) et ecs::NkWorld (NKECS, le monde des entités et des composants). Ici, deux
// espaces de noms imitent ce doublon.
#include <cstdio>

namespace collision {
    // Le monde des corps : on y ajoute des formes, il calcule les paires en contact.
    class NkWorld {
        public:
            int AddBody() { return ++mCorps; }
            int BodyCount() const { return mCorps; }

        private:
            int mCorps = 0;
    };
}

namespace ecs {
    // Le monde des entités : on y crée des entités, il range leurs composants.
    class NkWorld {
        public:
            unsigned CreateEntity() { return ++mEntites; }
            unsigned EntityCount() const { return mEntites; }

        private:
            unsigned mEntites = 0;
    };
}

// Un fichier qui a besoin des deux, comme NkPhysicsWorld.h et un système ECS réunis.
using namespace collision;
using namespace ecs;

int main() {
    // Usage non qualifié : le compilateur refuse, il ne sait pas lequel des deux.
    //     NkWorld monde;
    // clang : error: reference to 'NkWorld' is ambiguous
    // Gardé en commentaire pour que le fichier rendu compile ; le message complet est dans reponse.txt.

    // Correction : on qualifie, et chaque lecteur sait duquel on parle.
    collision::NkWorld physique;
    ecs::NkWorld scene;
    physique.AddBody();
    physique.AddBody();
    scene.CreateEntity();
    std::printf("collision::NkWorld : %d corps\n", physique.BodyCount());
    std::printf("ecs::NkWorld       : %u entite\n", scene.EntityCount());
    return 0;
}
