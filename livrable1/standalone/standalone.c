//
//  standalone.c
//  Avalam: Challenge Moteur de Jeux
//
//  Crée par Urban Prevost, Romain Lefebvre, Paul Bezeau et Gaston Pelletier le 03/02/21
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cJSON.h>
#include <avalam.h>
#include <topologie.h>

#ifdef __DEBUG__
    #define DBG 1
#else
    #define DBG 0
#endif

// MARK: Constantes
#define DEFAULT_FICHIER_NOM "../build/web/data/refresh-data.js"

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
int main(int argc, char *argv[]) {
    fichierNom = (char*)malloc(strlen(DEFAULT_FICHIER_NOM)+1); // nom du fichier d'écriture en sortie
    T_Score score = { 0, 0, 0, 0 }; // score des rouges et jaunes
    octet coupOrigine = 0, coupDestination = 0;
    int trait = JAU; // 1 pour jaune, 2 pour rouge

    // 1. lecture du fichier de sortie à l'execution du programme
    switch (argc-1) {
    	case 1:
            fichierNom = (char*)realloc(fichierNom, strlen(argv[1])+1);
            strcpy(fichierNom, argv[1]);
            break;
    	default:
            strcpy(fichierNom, DEFAULT_FICHIER_NOM);
            break;
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
        printf(" -- Au tour des %s --\n", (trait == 1) ? "Jaune (j)" : "Rouge (r)");

        // b. récupérer l'origine et la destination de la pile à déplacer
        do {
            T_Tuple etat = { 0, 0 };
            int temp;

            // tant que la saisie de l'utilisateur n'est pas correcte
	        while (!(etat.val0 & etat.val1)) {
		        printf("Position de la pile à déplacer: ");
		        etat.val0 = scanf("%hhd", &coupOrigine);
                getchar(); // suppresion du retour à ligne saisie par l'utilisateur

                printf("Destination de la pile à déplacer: ");
                etat.val1 = scanf("%hhd", &coupDestination);

                if (!(etat.val0 & etat.val1)) {
                    while ((temp=getchar()) != EOF && temp != '\n');
                    fprintf(stderr, "%serreur: entrée non définie%s\n\n", "\x1B[31m", "\x1B[0m");
                }
	        }
            printf("\n");
        } while (!estValide(pos, coupOrigine, coupDestination));

        // c. déplacer le pion joué
        pos = jouerCoup(pos, coupOrigine, coupDestination);

        // d. changer le trait
        trait = (trait == JAU) ? ROU : JAU;

        // e. mettre à jour le score pour les deux joueurs
        score = evaluerScore(pos);

        // f. mise à jour du score et de la position des pions dans le fichier de sortie
        creationjs(pos, score, trait);

        // g. mode debug
        if (DBG) {
            printf("%s", "\x1B[33m");
            printf("[     trait      ]   %d\n", trait);
            printf("[     scoreJ     ]   %d\n", score.nbJ);
            printf("[     scoreJ5    ]   %d\n", score.nbJ5);
            printf("[     scoreR     ]   %d\n", score.nbR);
            printf("[     scoreR5    ]   %d\n", score.nbR5);
            printf("[ coups restants ]   %d\n", getCoupsLegaux(pos).nb);
            printf("[     en tête    ]   %s\n", (score.nbJ+score.nbJ5) > (score.nbR+score.nbR5) ? STR_J: STR_R);
            printf("%s\n", "\x1B[0m");
        }
    }

    // 4. affichage du vainqueur
    printf("Bon jeux - ");
    if (score.nbJ > score.nbR) { // cas où les jaunes sont vainqueur
        printf("Les jaunes (J) sont vainqueurs\n");
    } else if (score.nbJ < score.nbR) { // cas où les rouges sont vainqueur
        printf("Les rouges (R) sont vainqueurs\n");
    } else { // cas d'égalité
        if (score.nbJ5 > score.nbR5) { // cas où les jaunes sont vainqueur
            printf("Les jaunes (J) sont vainqueurs\n");
        } else if (score.nbJ5 < score.nbR5) { // cas où les rouges sont vainqueur
            printf("Les rouges (R) sont vainqueurs\n");
        } else { // cas d'égalité
            printf("Egalité entre les J et R\n");
        }
    }

    // 5. affiche du score en fin de partie
    afficherScore(score);
    printf("\n");

    // 6. nettoyage global
    free(fichierNom);
    fichierNom = NULL;
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
    cJSON_AddItemToObject(root, STR_TURN, cJSON_CreateNumber(trait));
    cJSON_AddItemToObject(root, STR_SCORE_J, cJSON_CreateNumber(score.nbJ));
    cJSON_AddItemToObject(root, STR_SCORE_J5, cJSON_CreateNumber(score.nbJ5));
    cJSON_AddItemToObject(root, STR_SCORE_R, cJSON_CreateNumber(score.nbR));
    cJSON_AddItemToObject(root, STR_SCORE_R5, cJSON_CreateNumber(score.nbR5));

        // b. ajout de la position des pions
    cJSON_AddItemToObject(root, STR_COLS, cols);
    for (unsigned int i = 0; i < NBCASES; ++i) {
        cJSON_AddItemToArray(cols, col = cJSON_CreateObject());
        cJSON_AddItemToObject(col, STR_NB, cJSON_CreateNumber(pos.cols[i].nb));
        cJSON_AddItemToObject(col, STR_COULEUR, cJSON_CreateNumber(pos.cols[i].couleur));
    }

    // 2. création d'un string js contenant une entete ouvrante et fermante ainsi que le string json
    char *jsonString = cJSON_Print(root);
    char* jsString = (char*)malloc(strlen(JS_ENTETE_OUVRANT)+strlen(jsonString)*CJSON_BUFFER_TAILLE+strlen(JS_ENTETE_FERMANT));

        // a. copie de l'entete javascript ouvrante dans le string final
    strcpy(jsString, JS_ENTETE_OUVRANT);

        // b. copie de l'information de jeux sous format json dans le string final
    strcat(jsString, jsonString);

        // c. copie de l'entete javascript fermante dans le string final
    strcat(jsString, JS_ENTETE_FERMANT);

    // 3. enregistrement du string json dans le fichier d'écriture
        // a. ouverture ou créeation du fichier de sortie
    fichier = fopen(fichierNom, FICHIER_PERM_WRITE);

        // b. vérification de la possibilité d'écriture
    if (fichier == NULL) throwFile(fichierNom);
    else fputs(jsString, fichier);

    // 4. nettoyage
    free(jsonString);
    free(jsString);
    jsonString = NULL;
    jsString = NULL;
    cJSON_Delete(root);
    fclose(fichier);
}
