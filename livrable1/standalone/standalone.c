#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cJSON.h>
#include <avalam.h>
#include <topologie.h>

// MARK: Constantes
#define DEFAULT_FICHIER_NOM "../web/exemples/refresh-data.js"
#define FICHIER_PERM "w"
#define DEFAULT_JSON_TAILLE 2048
#define REP_OUI 'Y'
#define REP_NON 'n'

// MARK: Prototypes de fonctions
void creationjs(T_Position pos, T_Score score, int trait);

// MARK: Variables globales
char *fichierNom = NULL;

// MARK: Main
/**
standalone.exe : Jeu à deux joueurs, saisie des coups au clavier, affichage par avalam-refresh

Notes:
1. Permet de jouer à deux joueurs
2. Saisie des coups chacun son tour au clavier
3. Produit un fichier json avec la position et le score (a chaque coup)
4. Le nom du fichier peut être passé en ligne de commandes
5. Détecte la fin de la partie et affiche le score à l’écran
*/
int main(int argc, char * argv[]) {
    fichierNom = malloc(sizeof(char*)); // nom du fichier d'écriture en sortie
    T_Score score = { 0, 0, 0, 0 }; // score des rouges et jaunes
    octet coupOrigine = 0, coupDestination = 0;
    int trait = 0; // 0 pour jaune, 1 pour rouge

    // 1. lecture du fichier de sortie à l'execution du programme
    switch (argc-1) {
    	case 1: fichierNom = argv[1]; break;
    	default: fichierNom = DEFAULT_FICHIER_NOM; break;
    }
    printf("Le fichier d'écriture sera: %s\n\n", fichierNom);

    // 2. écriture initiale
        // a. récupérer la position initiale
    T_Position pos = getPositionInitiale();

        // b. écriture de la position initiale des pions dans le fichier de sortie
    creationjs(pos, score, trait);

    // 3. tant qu'aucun joueur n'a gagné
    while (getCoupsLegaux(pos).nb != 0) {
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

        // f. mise à jour du score et de la position des pions dans le fichier de sortie
        creationjs(pos, score, trait);
    }

    // 4. nettoyage global
    free(fichierNom);
    return EXIT_SUCCESS;
}

// MARK: Implémentation
/**
Créer un fichier au nom et répertoire spécifié contenant le score et la position des pion au format JSON

- Paramètre pos: contient la position des pions sur le jeux
- Paramètre score: contient le score pour la couleur jaune et rouge
- Paramètre trait: indique le tour à jouer (rouge (r) ou jaune (j))
*/
void creationjs(T_Position pos, T_Score score, int trait) {
	FILE *fichier; // flux d'écriture pour le fichier standalone.js
	cJSON *root, *cols, *col; // cJSON

	// 1. création d'un string json enregistrant le score et la position des pions
    root = cJSON_CreateObject(); // object json racine
    cols = cJSON_CreateArray(); // tableau json des positions

        // a. ajout du score
    cJSON_AddItemToObject(root, "trait", cJSON_CreateNumber(trait));
    cJSON_AddItemToObject(root, "scoreJ", cJSON_CreateNumber(score.nbJ));
    cJSON_AddItemToObject(root, "scoreJ5", cJSON_CreateNumber(score.nbJ5));
    cJSON_AddItemToObject(root, "scoreR", cJSON_CreateNumber(score.nbR));
    cJSON_AddItemToObject(root, "scoreR5", cJSON_CreateNumber(score.nbR5));

        // b. ajout de la position des pions
    cJSON_AddItemToObject(root, "cols", cols);
    for (unsigned int i = 0; i < NBCASES; ++i) {
        cJSON_AddItemToArray(cols, col = cJSON_CreateObject());
        cJSON_AddItemToObject(col, "nb", cJSON_CreateNumber(pos.cols[i].nb));
        cJSON_AddItemToObject(col, "couleur", cJSON_CreateNumber(pos.cols[i].couleur));
    }

        // c. ajout de l'entête au fichier json
    char jsonString[DEFAULT_JSON_TAILLE] = "traiterJson("; // string json final
    strcat(jsonString, cJSON_Print(root));
    strcat(jsonString, ");\n");

    // 2. enregistrement du string json dans le fichier d'écriture
        // a. ouverture ou créeation du fichier de sortie
    fichier = fopen(fichierNom, FICHIER_PERM);

        // b. vérification de la possibilité d'écriture
    if (fichier == NULL) {
        fprintf(stderr, "%serreur: impossible d'ouvrir le fichier %s\n", "\x1B[31m", fichierNom);
        exit(EXIT_FAILURE);
    } else {
        fprintf(fichier, "%s", jsonString);
    }

    // 3. nettoyage
    cJSON_Delete(root);
    fclose(fichier);
}
