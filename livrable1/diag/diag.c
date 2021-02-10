//
//  diag.c
//  Avalam: Challenge Moteur de Jeux
//
//  Crée par Urban Prevost, Romain Lefebvre, Paul Bezeau et Gaston Pelletier le 04/02/21
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <time.h>
#include <cJSON.h>
#include <avalam.h>
#include <topologie.h>

#ifdef __DEBUG__
    #define DBG 1
#else
    #define DBG 0
#endif

// MARK: Constantes
#define DEFAULT_FICHIER_NOM "../web/exemples/diag_initial.js"
#define FICHIER_NOM_DESCRIPTION "diag-desc.txt"
#define INTER_COMMAND "cat >"
#define LG_DESCRIPTION 45
#define REP_OUI 'Y'
#define REP_NON 'n'

// MARK: Prototypes de fonctions
void nvcol(cJSON *array, const int nb, const int couleur);
bool fenValide(char *fen);
char *rmNewline(char*);
int cversi(const char);
int aiversi(const int*, const int);
int fenversi(const char);

// MARK: Variables globales
int trait = 0; // 0 pour vide, 1 pour jaune, 2 pour rouge

// MARK: Main
int main(int argc, char *argv[]) {
    FILE *fichier; // flux d'écriture pour le fichier diag.js
    cJSON *root, *cols, *col; // cJSON
    char *fichierNom = (char*)malloc(LG_DESCRIPTION+1); // nom du fichier de sortie
    char *description = (char*)malloc(LG_DESCRIPTION+1); // description de la partie représentée par le fen
    char *fen = (char*)malloc(sizeof(char)); // fen de la partie
    octet numDiag; // numéro de diagramme

    // initialisation
    srand(time(NULL));

    // 1. lecture du numéro de diagramme et du fen à l'execution du programme
    if ((argc-1) > 2 || (argc-1) < 1) {
        throw("erreur: synthaxe attendue: diag.exe <numero_de_diagramme> \"<position_type_FEN>\"");
    }

    // 2. vérification de la saisie du numéro de digramme
    bool diagPremier = true, diagSecond = true;
    unsigned int i=0;

    while (argv[i]) { if (strlen(argv[i++]) == 0) throwInput(); }
    i=0;

        // a. si diagNum est le premier argument
    if ((argc-1) == 2) {
        while (argv[1][i]) { if (!isdigit(argv[1][i++])) { diagPremier = false; break; } }
    } else {
        diagPremier = false;
    }
    i=0;

        // b. si diagNum est le second argument
    if (!diagPremier && (argc-1) == 2) {
        while (argv[2][i]) { if (!isdigit(argv[2][i++])) { diagSecond = false; break; } }
    } else {
        diagSecond = false;
    }

    // 3. assignation du numéro de diagramme selon sa position
    int fenIndex;
    switch (diagPremier) {
        case true:
        // a. le numéro de diagramme est le premier argument
        sscanf(argv[1], "%hhd", &numDiag);

        // b. index de la fen dans argv
        fenIndex = 2;
        break;

        case false:
        // a. le numéro de diagramme est le second argument
        if (diagSecond) sscanf(argv[2], "%hhd", &numDiag);

        // b. le numéro de diagramme n'est pas été donné
        else numDiag = rand() % 100;

        // c. index de la fen dans argv
        fenIndex = 1;
        break;
    }

    // 4. assignation de la fen si valide
    fen = (char*)realloc(fen, strlen(argv[fenIndex])+1);
    if (fenValide(argv[fenIndex])) strcpy(fen, argv[fenIndex]);
    else throwInput();

    // 5. demande du nom de fichier de json sortie
    char option;
    char *p;
    printf("Saisir nom du fichier : ");
    fgets(fichierNom, LG_DESCRIPTION, stdin);
    if(strcmp(fichierNom, "\n")==0){
        strcpy(fichierNom, DEFAULT_FICHIER_NOM);
    }
    if ((p = strchr(fichierNom, '\n')) != NULL) *p = '\0';
    strcmp(fichierNom, p);
    printf("\nLe fichier de sortie sera: %s\n\n", fichierNom);

    // 6. demander à l'utilisateur une chaine de description
    char *command = (char*)malloc(strlen(INTER_COMMAND)+strlen(FICHIER_NOM_DESCRIPTION)+1);
    strcpy(command, INTER_COMMAND);
    strcat(command, FICHIER_NOM_DESCRIPTION);
    option = REP_NON;

    char *description2 = (char*)malloc(LG_DESCRIPTION+1);
    strcpy(description, "\0");

    printf("Chaine de description (%d caractères max): \n", LG_DESCRIPTION);
    fgets(description, LG_DESCRIPTION, stdin);
    if ((p = strchr(description, '\n')) != NULL) *p = '\0';
    strcmp(description, p);
    //printf("%s\n", description);
    fgets(description2, LG_DESCRIPTION, stdin);
    if ((p = strchr(description2, '\n')) != NULL) *p = '\0';
    strcmp(description2, p);
    strcat(description, description2);
    //printf("%s\n", description);
    if(strcmp(description, "\0")){
        //printf("Chaine de description (%d caractères max): \n", LG_DESCRIPTION);
        system(command); // initiation d'une ligne de commande intéractive
        fichier = fopen(FICHIER_NOM_DESCRIPTION, FICHIER_PERM_READ);
        fgets(description, LG_DESCRIPTION+1, fichier);
        fclose(fichier);
        free(command);
        remove(FICHIER_NOM_DESCRIPTION);
        printf("\n");
    }

    // 7. génération d'un string json
        // a. création d'un string json enregistrant le trait, description et fen de la partie
    root = cJSON_CreateObject(); // object json racine
    cols = cJSON_CreateArray(); // tableau json des positions

    cJSON_AddItemToObject(root, STR_TURN, cJSON_CreateNumber(trait));
    cJSON_AddItemToObject(root, STR_NUMDIAG, cJSON_CreateNumber(numDiag));
    cJSON_AddItemToObject(root, STR_NOTES, cJSON_CreateString(description));
    cJSON_AddItemToObject(root, STR_FEN, cJSON_CreateString(fen));

        // b. traduction du fen en cols et ajout de la position des pions au json
    cJSON_AddItemToObject(root, STR_COLS, cols);

    int chiffre[NBCASES];
    unsigned int inc=0, t;
    while (fen[inc] != ' ') {
        // est chiffre
        if (isdigit(fen[inc])) {
            int pos = inc; t=0;
            while (isdigit(fen[inc])) chiffre[t++] = cversi(fen[inc++]);
            for (unsigned int j = pos; j < pos+aiversi(chiffre, t); ++j) nvcol(cols, 0, 0);
        }
        // est minuscule
        if (islower(fen[inc])) nvcol(cols, fenversi(fen[inc++]), 1);
        // est majuscule
        if (isupper(fen[inc])) nvcol(cols, fenversi(fen[inc++]), 2);
    }

    // 8. écriture de l'information dans le fichier de sortie
        // a. création d'un string js contenant une entete ouvrante et fermante ainsi que le string json
    char *jsonString = cJSON_Print(root);
    char *jsString = (char*)malloc(strlen(JS_ENTETE_OUVRANT)+strlen(jsonString)*CJSON_BUFFER_TAILLE+strlen(JS_ENTETE_FERMANT));

        // b. copie de l'entete javascript ouvrante dans le string final
    strcpy(jsString, JS_ENTETE_OUVRANT);

        // c. copie de l'information de jeux sous format json dans le string final
    strcat(jsString, jsonString);

        // d. copie de l'entete javascript fermante dans le string final
    strcat(jsString, JS_ENTETE_FERMANT);

        // e. ouverture ou création du fichier diag.js
    fichier = fopen(fichierNom, FICHIER_PERM_WRITE);

        // f. vérification de la possibilité d'écriture
    if (fichier == NULL) throwFile(fichierNom);
    else fputs(jsString, fichier);

    // 9. mode debug
    if (DBG) {
        printf("%s", "\x1B[33m");
        printf("[  trait  ]   %d\n", trait);
        printf("[ numdiag ]   diag n°%d\n", numDiag);
        printf("[   fen   ]   %s\n", fen);
        printf("%s\n", "\x1B[0m");
    }

    // 10. nettoyage global
    free(fichierNom);
    free(description);
    free(description2);
    free(fen);
    free(jsonString);
    free(jsString);
    cJSON_Delete(root);
    fclose(fichier);
    return EXIT_SUCCESS;
}

// MARK: Implémentation
/**
Crée une nouvelle objet contenant un attribut nombre et couleur à l'intérieur d'un array donnée

- Paramètre array: auxquel on souhaite joindre l'objet crée
- Paramètre nb: à écrire danas l'objet crée (correspond aux nombre de pions dans une pile donnée)
- Paramètre couleur: à écrire dans l'objet crée (correspond à la couleur en tête de pile de pions)
*/
void nvcol(cJSON *array, const int nb, const int couleur) {
    cJSON *col = cJSON_CreateObject();
    cJSON_AddItemToArray(array, col);
    cJSON_AddItemToObject(col, STR_NB, cJSON_CreateNumber(nb));
    cJSON_AddItemToObject(col, STR_COULEUR, cJSON_CreateNumber(couleur));
}
/**
Valide la fen donnée en entrée selon le critère de somme est égal à 48 et trait jaune (j) ou rouge (r)

- Paramètre fen: à faire valider par la fonction
- Retourne: un boolean indiquant si la chaine est valide ou non
*/
bool fenValide(char *fen) {
    unsigned int i=0, j=0, compte=0;
    bool valide = false;

    // validation du nombres de pions donné par la fen
    int chiffre[NBCASES];
    while (fen[i] != ' ' && fen[i] != '\n' && fen[i] != '\0') {
        if (!isdigit(fen[i])) {
            ++compte;
        } else {
            while (isdigit(fen[i])) chiffre[j++] = cversi(fen[i++]);
            compte += aiversi(chiffre, j);
            --i;
        }
        j=0;
        ++i;
    }

    // validation du trait
    if (fen[i+1] == 'r' || fen[i+1] == 'j') valide = true;
    trait = (fen[i+1] == 'r') ? 2 : 1;

    // si le compte vaut 48 et le trait est valide alors la fen est valide
    return (compte == NBCASES && valide);
}
/**
Supprime le saut de ligne présent dans la plupart des strings (le \n)

- Paramètre s: correspond au string pour lequel on souhaite supprimer le saut de ligne
- Retourne: le string d'entrée sans le saut de ligne s'il existait
*/
char *rmNewline(char s[]) {
    s = strstr(s, "\n");
    if (s != NULL) { strncpy(s, "\0", 1); }
    return s;
}
/**
Convertit un entier de type caractère vers un entier

- Paramètre c: le chiffre sous forme de caractère à convertir en entier
- Retourne: un entier qui correspond au caractère d'entrée convertit
*/
int cversi(const char c) {
    return c-'0';
}
/**
Convertit un tableau d'entier en entier
- Paramètre e: tableau d'entier à faire convertir
- Paramètre taille: du tableau à faire convertir
- Retourne: un entier qui correspond aux chiffres stockés dans chaque cases du tableau d'entiers
*/
int aiversi(const int e[], const int taille) {
    int resultat = 0;
    for (unsigned int i = 0; i < taille; ++i) {
        resultat *= 10;
        resultat += e[i];
    }
    return resultat;
}
/**
Convertiseur d'un caractère de la fen en son chiffre équivalent

- Paramètre c: caractère de la fen à faire convertir
- Retourne: un entier qui correspond au caractère d'entrée convertit
*/
int fenversi(const char c) {
    switch (c) {
        case 'u': return 1;
        case 'd': return 2;
        case 't': return 3;
        case 'q': return 4;
        case 'c': return 5;
        case 'U': return fenversi('u');
        case 'D': return fenversi('d');
        case 'T': return fenversi('t');
        case 'Q': return fenversi('q');
        case 'C': return fenversi('c');
    }
}
