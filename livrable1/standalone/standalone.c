#include <stdio.h>
#include <avalam.h>
#include <topologie.h>

#define FICHIER_NOM "standalone.js"
#define FICHIER_PERM "w"

int main() {
    FILE *fichier; // flux d'écriture pour le fichier standalone.js
    int trait = 0; // 0 pour jaune, 1 pour rouge
    T_Score score = { 0, 0, 0, 0 }; // score des rouges et jaunes

    // 1. créer le fichier standalone.js
    fichier = fopen(FICHIER_NOM, FICHIER_PERM);

    // 2. récupérer la position initiale
    T_Position pos = getPositionInitiale();

    // 3. tant qu'aucun joueur n'a gagné
    while (getCoupsLegaux(pos).nb != 0) {
        octet coupOrigine, coupDestination;

        // a. affichage de la couleur à jouer (jaune ou rouge)
        printf(" -- Au tour des %s --\n", (trait) ? "Rouge (R)" : "Jaune (J)");

        // b. récupérer l'origine et la destination de la pile à déplacer
        do {
            printf("Position de la pile à déplacer: ");
            scanf("%hhd", &coupOrigine);
            printf("Destination de la pile à déplacer: ");
            scanf("%hhd", &coupDestination);
            printf("\n");
        } while (!estValide(pos, coupOrigine, coupDestination));

        // c. déplacer le pion joué
        pos = jouerCoup(pos, coupOrigine, coupDestination);

        // d. changer le trait
        trait = (trait) ? 0 : 1;

        // e. mettre à jour le score pour les deux joueurs
        score = evaluerScore(pos);

        // f. enregistrer le déplacement dans le fichier standalone.js en format json
            // - créer l'entete du fichier json
        char *jsonString = ""; // string json final

            // - écrire le string json dans le fichier standalone.js
        fputs(jsonString, fichier);
    }
}
