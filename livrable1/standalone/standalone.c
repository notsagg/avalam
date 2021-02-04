#include <stdio.h>
#include <stdlib.h>
#include <cJSON.h>
#include <string.h>
#include <avalam.h>
#include <topologie.h>

#define DEFAULT_FICHIER_NOM "../web/exemples/refresh-data.js"
#define DEFAULT_EXTENSION ".js"
#define FICHIER_PERM "w"
#define DEFAULT_JSON_TAILLE 2048

void creationjs(T_Position pos, T_Score score, int trait);
char fichierDestination[DEFAULT_JSON_TAILLE]; //="../web/exemples/";

int main(int argc, char * argv[]) {

    char *nomFichier = malloc(sizeof(char*));
    T_Score score = { 0, 0, 0, 0 }; // score des rouges et jaunes
    int trait = 0; // 0 pour jaune, 1 pour rouge
    octet coupOrigine = 0, coupDestination = 0;

    switch(argc-1){
    	case 1:
    		nomFichier = argv[1];
    		break;
    	default:
    		nomFichier = DEFAULT_FICHIER_NOM;
    		break;
    }
    
    strcat(fichierDestination, nomFichier);
    printf("Le fichier sera dans: %s\n\n", nomFichier);
	
    // 2. récupérer la position initiale
    T_Position pos = getPositionInitiale();
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
        
        creationjs(pos, score, trait);
    }

    // 4. nettoyage global
    free(nomFichier);
    return EXIT_SUCCESS;
}

void creationjs(T_Position pos, T_Score score, int trait){

	FILE *fichier; // flux d'écriture pour le fichier standalone.js
	cJSON *root, *cols, *col; // cJSON

	//création d'un string json enregistrant le score et la position des pions
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
    fichier = fopen(fichierDestination, FICHIER_PERM);

    // - vérifier qu'on peut y écrire puis écrire
    if (fichier == NULL) {
        printf("Erreur d'ouverture du fichier");
    } else {
        fprintf(fichier, "%s", jsonString);
    }
    
    // h. nettoyage
    cJSON_Delete(root);
    fclose(fichier);
}
