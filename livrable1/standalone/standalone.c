#include <stdio.h>
#include <stdlib.h>
#include <cJSON.h>
#include <string.h>
#include <avalam.h>
#include <topologie.h>

#define DEFAULT_FICHIER_NOM "standalone.js"
#define FICHIER_PERM "w"
#define DEFAULT_JSON_TAILLE 2048

int main() {
    FILE *fichier; // flux d'écriture pour le fichier standalone.js
    cJSON *root, *cols, *col; // cJSON
    char *nomFichier = malloc(sizeof(char*));
    T_Score score = { 0, 0, 0, 0 }; // score des rouges et jaunes
    int trait = 0; // 0 pour jaune, 1 pour rouge

    // 1. demander à l'utilisateur le nom du fichier .js à écrire
    char option;
    printf("Souhaitez vous nommer le fichier de sortie? [Y/n] ");
    scanf("%c", &option);

    if (option == 'Y') {
        printf("Comment souhaitez vous le nommer: ");
        scanf("%s", nomFichier);
    } else if (option == 'n') {
        nomFichier = DEFAULT_FICHIER_NOM;
    } else {
        printf("erreur: entrée non définie\n");
        return EXIT_FAILURE;
    }
    printf("Le nom du fichier de sortie sera: %s\n\n", nomFichier);

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

        // f. création d'un string json enregistrant le score et la position des pions
        root = cJSON_CreateObject(); // object json racine
        cols = cJSON_CreateArray(); // tableau json des positions

            // - ajout du score
        cJSON_AddItemToObject(root, "trait", cJSON_CreateNumber(trait));
        cJSON_AddItemToObject(root, "scoreJ", cJSON_CreateNumber(score.nbJ));
        cJSON_AddItemToObject(root, "scoreJ5", cJSON_CreateNumber(score.nbJ5));
        cJSON_AddItemToObject(root, "scoreR", cJSON_CreateNumber(score.nbR));
        cJSON_AddItemToObject(root, "scoreR5", cJSON_CreateNumber(score.nbR5));

            // - ajout de la position des pions
        cJSON_AddItemToObject(root, "cols", cols);

        for (unsigned int i = 0; i < NBCASES; ++i) {
            cJSON_AddItemToArray(cols, col = cJSON_CreateObject());
            cJSON_AddItemToObject(col, "nb", cJSON_CreateNumber(pos.cols[i].nb));
            cJSON_AddItemToObject(col, "couleur", cJSON_CreateNumber(pos.cols[i].couleur));
        }

            // - ajout de l'entete au fichier json
        char jsonString[DEFAULT_JSON_TAILLE] = "traiterJson("; // string json final
        strcat(jsonString, cJSON_Print(root));
        strcat(jsonString, ");\n");

        // g. enregistrer le string json dans le fichier standalone.js
            // - ouvrir ou créer le fichier standalone.js
        fichier = fopen(nomFichier, FICHIER_PERM);

            // - vérifier qu'on peut y écrire puis écrire
        if (fichier == NULL) {
            printf("Erreur d'ouverture du fichier %s", nomFichier);
        } else {
            fprintf(fichier, "%s", jsonString);
        }

        // h. nettoyage
        cJSON_Delete(root);
        fclose(fichier);
    }

    // 4. nettoyage global
    free(nomFichier);
    return EXIT_SUCCESS;
}
