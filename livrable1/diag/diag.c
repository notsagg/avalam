#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
#include <string.h>
#include <cJSON.h>
#include <avalam.h>

// MARK: Constantes
#define DEFAULT_FICHIER_NOM "diag.js"
#define DEFAULT_EXTENSION ".js"
#define FICHIER_PERM "w"
#define DEFAULT_JSON_TAILLE 2048
#define LG_DESCRIPTION 55
#define REP_OUI 'Y'
#define REP_NON 'n'

// MARK: Prototypes de fonctions
void throw();
void nvcol(cJSON*, const int, const int);
int arrIntVersInt(const int*);
int nbcarVersInt(const char);
char *rmNewLine(char *);
bool fenValide(char *);

// MARK: Main
int main() {
    FILE *fichier; // flux d'écriture pour le fichier diag.js
    cJSON *root, *cols, *col; // cJSON
    char *nomFichier = malloc(sizeof(char*)); // nom du fichier de sortie
    char *description = malloc(sizeof(char*)); // description de la partie représentée par le fen
    char *fen = malloc(sizeof(char*)); // fen de la partie
    int trait; // 0 pour vide, 1 pour jaune, 2 pour rouge

    // 1. demander à l'utilisateur le nom du fichier .js à écrire
    char option;
    printf("Souhaitez vous nommer le fichier de sortie? [%c/%c] ", REP_OUI, REP_NON);
    scanf("%c", &option);
    getchar();

    if (option == REP_OUI) {
        printf("Comment souhaitez vous le nommer: ");
        scanf("%s", nomFichier);
        strcat(nomFichier, DEFAULT_EXTENSION);
    } else if (option == REP_NON) {
        nomFichier = DEFAULT_FICHIER_NOM;
    } else {
        throw();
    }
    printf("Le fichier de sortie sera: %s\n\n", nomFichier);

    // 2. demander à l'utilisateur une chaine de description
    option = REP_NON;
    while (option != REP_OUI) {
        printf("Chaine de description (%d caractères max): ", LG_DESCRIPTION);
        fgets(description, LG_DESCRIPTION+1, stdin);
        rmNewLine(description); // supprimer le \n

        printf("La description sera: %s\n", description);
        printf("Valider? [%c/%c] ", REP_OUI, REP_NON);
        scanf("%c", &option);
        getchar();
    }

    // 3. demander à l'utilisateur le fen
    printf("Numéro de diagramme de type FEN: ");
    fgets(fen, NBCASES+3, stdin);
    rmNewLine(fen); // supprimer le \n
    // if (!fenValide(fen)) throw(); // vérifier le fen tapé par l'utilisateur

    // 4. traduire le fen en trait
    unsigned int i = 0;
    while (fen[i] != ' ') { ++i; }

    switch (fen[i+1]) {
        case 'j': trait = 1; break;
        case 'r': trait = 2; break;
        default: throw(); break;
    }

    // 5. écrire l'information dans le fichier json
        // a. création d'un string json enregistrant le trait, description et fen de la partie
    root = cJSON_CreateObject(); // object json racine
    cols = cJSON_CreateArray(); // tableau json des positions

    cJSON_AddItemToObject(root, "trait", cJSON_CreateNumber(trait));
    // cJSON_AddItemToObject(root, "numDiag", cJSON_CreateNumber(numDiag));
    cJSON_AddItemToObject(root, "notes", cJSON_CreateString(description));
    cJSON_AddItemToObject(root, "fen", cJSON_CreateString(fen));

   // 7. ajout de l'entete au fichier json
    char jsonString[DEFAULT_JSON_TAILLE] = "traiterJson("; // string json final
    strcat(jsonString, cJSON_Print(root));
    strcat(jsonString, ");\n");

    // 8. écriture du string json dans le fichier diag.js
        // - ouverture ou création du fichier diag.js
    fichier = fopen(nomFichier, FICHIER_PERM);

        // - vérification de la possibilité d'écriture
    if (fichier == NULL) {
        printf("Erreur d'ouverture du fichier %s", nomFichier);
    } else {
        fprintf(fichier, "%s", jsonString);
    }

    // 8. nettoyage
    cJSON_Delete(root);
    fclose(fichier);
    free(description);
    free(fen);
    return EXIT_SUCCESS;
}

// MARK: Implémentation
/**
*/
void throw() {
    fprintf(stderr, "%serreur: entrée non définie\n", "\x1B[31m");
    exit(EXIT_FAILURE);
}
/**
*/
void nvcol(cJSON *array, const int nb, const int couleur) {
    cJSON *col = cJSON_CreateObject();
    cJSON_AddItemToArray(array, col);
    cJSON_AddItemToObject(col, "nb", cJSON_CreateNumber(nb));
    cJSON_AddItemToObject(col, "couleur", cJSON_CreateNumber(couleur));
}
/**
*/
int arrIntVersInt(const int e[]) {
    int resultat = 0, i = 0;
    while (e[i] != '\0') {
        resultat *= 10;
        resultat += e[i];
    }
    return resultat;
}
/**
 */
int nbcarVersInt(const char c) {
    switch (c) {
        case 'u': return 1;
        case 'd': return 2;
        case 't': return 3;
        case 'q': return 4;
        case 'c': return 5;
        case 'U': nbcarVersInt('u');
        case 'D': nbcarVersInt('d');
        case 'T': nbcarVersInt('t');
        case 'Q': nbcarVersInt('q');
        case 'C': nbcarVersInt('c');
    }
}
/**
*/
char *rmNewLine(char s[]) {
    s = strstr(s, "\n");
    if(s != NULL) { strncpy(s, "\0", 1); }
    return s;
}
/**
*/
bool fenValide(char *fen) {
    unsigned int i = 0, j = 0, compte = 0;
    bool valide = false;

    // nombre de pions
    int chiffre[NBCASES];
    while (fen[i] != ' ' && fen[i] != '\n') {
        if (!isdigit(fen[i])) ++compte;
        else chiffre[j++] = (int)fen[i];
        ++i;
    }
    compte += arrIntVersInt(chiffre);

    // trait
    if (fen[i+1] == 'r' || fen[i+1] == 'j') valide = true;

    return (compte == NBCASES && valide) ? true : false;
}