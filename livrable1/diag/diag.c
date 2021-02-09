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

// MARK: Constantes
#define DEFAULT_FICHIER_NOM "../build/web/data/diag-diag.js"
#define DEFAULT_JSON_TAILLE 2048
#define LG_DESCRIPTION 45
#define REP_OUI 'Y'
#define REP_NON 'n'

// MARK: Prototypes de fonctions
void creationjs(char *fen, char *description, octet numDiag, char *nomFichier);
void nvcol(cJSON*, const int, const int);
bool fenValide(char *);
char *rmNewline(char *);
int cversi(const char);
int aiversi(const int*, const int);
int fenversi(const char);

// MARK: Variables globales
char *fichierNom = NULL;
char *description = NULL;
char *fen = NULL;
octet numDiag;

// MARK: Main
int main(int argc, char *argv[]) {
    srand(time(NULL)); // initialisation
    char *nomFichier = (char*)malloc(LG_DESCRIPTION+1); // nom du fichier de sortie
    char *description = (char*)malloc(LG_DESCRIPTION+1); // description de la partie représentée par le fen
    char *fen = (char*)malloc(sizeof(char)); // fen de la partie

    // 1. lecture du numéro de diagramme et du fen à l'execution du programme
    if ((argc-1) > 2 || (argc-1) < 1) {
        throw("erreur: synthaxe attendue: diag.exe <numero_de_diagramme> \"<position_type_FEN>\"");
    }

    // 2. vérification de la saisir du numéro de digramme
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

    // 3. vérification de la position des pions de type "FEN"
    switch (diagPremier) {
        case true:

        // a. assignation du numéro de diagramme
        sscanf(argv[1], "%hhd", &numDiag);

        // b. assignation de la fen si valide
        fen = (char*)realloc(fen, strlen(argv[2])+1);
        if (fenValide(argv[2])) strcpy(fen, argv[2]);
        else throwInput(); break;

        case false:
        // a. assignation du numéro de diagramme
        if (diagSecond) sscanf(argv[2], "%hhd", &numDiag);
        else numDiag = rand() % 100;

        // b. assignation de la fen si valide
        fen = (char*)realloc(fen, strlen(argv[1])+1);
        if (fenValide(argv[1])) strcpy(fen, argv[1]);
        else throwInput(); break;
    }

    // 4. demande du nom de fichier de json sortie
    char option;
    printf("Souhaitez vous nommer le fichier de sortie? [%c/%c] ", REP_OUI, REP_NON);
    scanf("%c", &option);
    getchar();

    if (toupper(option) == REP_OUI) {
        printf("Comment souhaitez vous le nommer: ");
        fgets(nomFichier, LG_DESCRIPTION, stdin);
        rmNewline(nomFichier); // suppression du \n
    } else if (tolower(option) == REP_NON) {
        nomFichier = DEFAULT_FICHIER_NOM;
    } else {
        throwInput();
    }
    printf("Le fichier de sortie sera: %s\n\n", nomFichier);

    // 5. demander à l'utilisateur une chaine de description
    option = REP_NON;
    while (toupper(option) != REP_OUI) {
        printf("Chaine de description (%d caractères max): ", LG_DESCRIPTION);
        fgets(description, LG_DESCRIPTION+1, stdin);
        rmNewline(description); // suppression du \n

        printf("La description sera: %s\n", description);
        printf("Valider? [%c/%c] ", REP_OUI, REP_NON);
        scanf("%c", &option);
        getchar();
    }

    // 6. créer le fichier json de sortie
    creationjs(fen, description, numDiag, nomFichier);

    // 7. nettoyage global
    free(description);
    free(fen);
    return EXIT_SUCCESS;
}

// MARK: Implémentation
void creationjs(char *fen, char *description, octet numDiag, char *nomFichier) {
	FILE *fichier; // flux d'écriture pour le fichier diag.js
	cJSON *root, *cols, *col; // cJSON
	int trait; // 0 pour vide, 1 pour jaune, 2 pour rouge

	// 1. traduction de la fen en trait
    unsigned int i = 0;
    while (fen[i] != ' ') { ++i; }

    switch (fen[i+1]) {
        case 'j': trait = 1; break;
        case 'r': trait = 2; break;
        default: throwInput(); break;
    }

    // 2. écrire l'information dans le fichier json
        // a. création d'un string json enregistrant le trait, description et fen de la partie
    root = cJSON_CreateObject(); // object json racine
    cols = cJSON_CreateArray(); // tableau json des positions

    cJSON_AddItemToObject(root, STR_TURN, cJSON_CreateNumber(trait));
    cJSON_AddItemToObject(root, STR_NUMDIAG, cJSON_CreateNumber(numDiag));
    cJSON_AddItemToObject(root, STR_NOTES, cJSON_CreateString(description));
    cJSON_AddItemToObject(root, STR_FEN, cJSON_CreateString(fen));

    // 3. traduction du fen en cols et ajout de la position des pions au json
    cJSON_AddItemToObject(root, STR_COLS, cols);

    int chiffre[NBCASES];
    unsigned int inc=0, t=0;
    while (fen[inc] != ' ') {
        // est chiffre
        if (isdigit(fen[inc])) {
            int pos = inc;
            while (isdigit(fen[inc])) chiffre[t++] = cversi(fen[inc++]);
            for (unsigned int j = pos; j < pos+aiversi(chiffre, t); ++j) nvcol(cols, 0, 0);
        }

        // est minuscule
        if (islower(fen[inc])) nvcol(cols, fenversi(fen[inc++]), 1);

        // est majuscule
        if (isupper(fen[inc])) nvcol(cols, fenversi(fen[inc++]), 2);
    }

   // 4. ajout de l'entete au fichier json
    char jsonString[DEFAULT_JSON_TAILLE] = "traiterJson("; // string json final
    strcat(jsonString, cJSON_Print(root));
    strcat(jsonString, ");\n");

    // 5. écriture du string json dans le fichier diag.js
        // a. ouverture ou création du fichier diag.js
    fichier = fopen(nomFichier, FICHIER_PERM);

        // b. vérification de la possibilité d'écriture
    if (fichier == NULL) {
        printf("Erreur d'ouverture du fichier %s", nomFichier);
    } else {
        fprintf(fichier, "%s", jsonString);
    }

    // 6. nettoyage
    cJSON_Delete(root);
    fclose(fichier);
}
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
            compte += fenversi(fen[i]);
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
