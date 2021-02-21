//
//  diag.c
//  Avalam: Challenge Moteur de Jeux
//
//  Crée par Urban Prevost, Romain Lefebvre, Paul Bezeau et Gaston Pelletier le 04/02/21
//

#include <sys/stat.h>
#include <unistd.h>

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
int fenCount(char *fen);
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
    // 4. assignation de la fen
    fen = (char*)realloc(fen, strlen(argv[fenIndex])+1);
    strcpy(fen, argv[fenIndex]);
   
    //il faut tester si la premiere ligne de stdin est vide
    //char *test = (char *)malloc(LG_DESCRIPTION+1);
    //fgets(test,LG_DESCRIPTION+1,stdin);
    //printf("longueur de la première ligne de stdin : %ld\n",strlen(test));
    //free(test);
    //problème car si stdin est vide, démarre un mode interractif
    //il faut trouver le bon test
    if(isatty(fileno(stdin))){
        //stdin vide => pas de redirection
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
        system(command); // initiation d'une ligne de commande intéractive
        fichier = fopen(FICHIER_NOM_DESCRIPTION, FICHIER_PERM_READ);
        char carac;
        i =0;
        do{
            carac = fgetc(fichier);
            description[i]=carac;
            i++;
        } while (carac!= EOF && i<LG_DESCRIPTION);
        i=0;
        while (description[i]!='\0') i++;
        description[i-1]='\0';//suppression du ctrl+d
        fclose(fichier);
        free(command);
        remove(FICHIER_NOM_DESCRIPTION);
        printf("\n");
        free(description2);
    }else{
        //stdin non vide =>redirection
        //le fichier a été écris avant => il faut se placer au début de stdin, car on s'est déplacé 
        //pour tester si stdin était vide
        fseek(stdin,0,SEEK_SET);
        char *recup = (char*)malloc(LG_DESCRIPTION+1);
        do{
            fgets(recup,LG_DESCRIPTION+1,stdin);
            recup = rmNewline(recup);
        }while(strcmp(recup, "\0")==0);
        strcpy(fichierNom,recup);
        free(recup);
        char carac;
        int i=0;
        int y =0;
        do{
            
            carac = fgetc(stdin);
            description[i] = carac;
            i++;
            if(strcmp(&carac, "\n")==1){
                y++;
            }
        }while(carac != EOF && strlen(description)<LG_DESCRIPTION+1 && y<2);
        description[strlen(description)-1] = '\0';
        printf("Le fichier de sortie sera : %s\n", fichierNom);
        printf("description : \n\"%s\"\n",description);
    }
    //remplacement des \n dans la description par des <br />pour faire un retour à la ligne dans le html
    

    //recherche du trait dans la fen, fonctionne si il n'y a pas d'espace
    i=0;
    while(fen[i]!='r' && fen[i]!='j' && i<NBCASES) i++;
    switch (fen[i])
    {
    case 'r': trait = 2;
        break;
    case 'j': trait = 1;
        break;
    default: trait =0;
        break;
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
    unsigned int t;
    i=0;
    int compte = fenCount(fen);
    while(fen[i]!=' '){
        // est chiffre
        if (isdigit(fen[i])) {
            t=0;
            int debut = i;
            int j;
            while (isdigit(fen[i])){ 
                chiffre[t] = cversi(fen[i]);
                t++;i++;
            }
            for (j = debut; j < debut+aiversi(chiffre, t); ++j) nvcol(cols, 0, 0);
            i--;
        }
        // est minuscule
        else if (islower(fen[i])){ 
            if(fenversi(fen[i])==0) nvcol(cols,fenversi(fen[i]),0);
            else nvcol(cols, fenversi(fen[i]), 1);
        }
        // est majuscule
        else{ 
            if(fenversi(fen[i])==0) nvcol(cols,fenversi(fen[i]),0);
            else nvcol(cols, fenversi(fen[i]), 2);
        //caractère non attendu
        }
        i++;
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
Indique le nombre de cases que contiendra le plateau avec cette fen, en fonction des caractères rencontrés

- Paramètre fen: chaîne sur laquelle on compte le nombre de cases
- Retourne: un entier indiquant le nombre total de cases 
*/
int fenCount(char *fen) {
    unsigned int i=0, j=0, compte=0;
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
    return compte;
}
/**
Supprime le saut de ligne présent dans la plupart des strings (le \n)

- Paramètre s: correspond au string pour lequel on souhaite supprimer le saut de ligne
- Retourne: le string d'entrée sans le saut de ligne s'il existait
*/
char *rmNewline(char s[]) { 
    //printf("%s\n", s);
    int i=0;
    while(s[i]!='\n' && s[i]!='\0')i++;
    s[i]='\0';   
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
        default : return 0;
    }
}