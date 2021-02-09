// MARK: Debug
#ifdef __DEBUG__
	#define printf0(p) printf(p)
	#define printf1(p,q) printf(p,q)
	#define printf2(p,q,r) printf(p,q,r)
	#define printf3(p,q,r,s) printf(p,q,r,s)
	#define printf4(p,q,r,s,t) printf(p,q,r,s,t)
	#define whoamid(p) whoami(p)
	#define whopd(p) whop(p)
	#define whojd(p) whoj(p)
	#define whoamjd() whoamj()
#else
	#define printf0(p)
	#define printf1(p,q)
	#define printf2(p,q,r)
	#define printf3(p,q,r,s)
	#define printf4(p,q,r,s,t)
	#define whoamid(p)
	#define whoamjd()
	#define whopd(p)
	#define whojd(p)
#endif

// MARK: Constantes
#define FAUX 0
#define VRAI 1
#define NBCASES 48

// Vainqueurs des parties et joueurs au trait
#define EGALITE 0
#define VIDE 0
#define JAU 1
#define ROU 2

#define COLNAME(c) ((c == ROU) ? "rouge" : "jaune")

// Exports JSON
#define FICHIER_PERM "w"
#define JS_ENTETE_OUVRANT "traiterJson("
#define JS_ENTETE_FERMANT ");\n"
#define CJSON_BUFFER_TAILLE 256

#define STR_NB "nb"
#define STR_COULEUR "couleur"
#define STR_TURN "trait"
#define STR_COLS "cols"

#define STR_FEN "fen"
#define STR_SCORE_J "scoreJ"
#define STR_SCORE_J5 "scoreJ5"
#define STR_SCORE_R "scoreR"
#define STR_SCORE_R5 "scoreR5"

#define STR_NUMDIAG "numDiag"
#define STR_NOTES "notes"

#define STR_COUPS "\"coups\""
#define STR_ORIGINE "\"o\""
#define STR_DESTINATION "\"d\""
#define STR_J "j"
#define STR_R "r"
#define STR_JOUEURS "\"joueurs\""

#define STR_NOM "\"nom\""
#define STR_SCORE "\"score\""
#define STR_RONDES "\"rondes\""
#define STR_RONDE "\"ronde\""
#define STR_PARTIES "\"parties\""
#define STR_RESULTAT "\"resultat\""
#define STR_STATUT "\"statut\""

// Couleurs
#define CL_BLANC "\x1B[0m"
#define CL_JAUNE "\x1B[33m"
#define CL_ROUGE "\x1B[31m"
#define CL_VERT "\x1B[32m"

// MARK: Macros
// vérification appels systèmes
#define CHECK_IF(sts,val,msg) \
if ((sts) == (val)) {fprintf(stderr,"erreur appel systeme\n");perror(msg); exit(-1);}

#define CHECK_DIF(sts,val,msg) \
if ((sts) != (val)) {fprintf(stderr,"erreur appel systeme\n");perror(msg); exit(-1);}

// MARK: Types
typedef unsigned char octet;

typedef struct {
	octet nbJ; 	octet nbJ5; // total et piles de 5 pour les jaunes
	octet nbR;	octet nbR5; // total et piles de 5 pour les rouges
} T_Score;

typedef struct {
	octet nb;
	octet cases[8]; // il peut y en avoir moins... ils vaudront 0
} T_Voisins;

typedef struct {
	octet nb; 			// nb d'elts dans la colonne
	octet couleur;  // couleur du sommet
} T_Colonne;

typedef struct {
	octet trait;
	// octet numCoup; // A ajouter
	T_Colonne cols[NBCASES];
} T_Position;

typedef struct {
	octet origine;
	octet destination;
} T_Coup;

typedef struct {
	int nb;
	T_Coup coups[8*NBCASES];
} T_ListeCoups;

typedef struct {
    int val0;
    int val1;
} T_Tuple;

// MARK: Déclaration - Prototypes de Fonctions
// Manipulation du jeux
/// Le nombre de voisins autour d'une case donnée
octet nbVoisins(octet numCase);
/// Les numéros de cases des voisins d'un numéro de cases donné
T_Voisins getVoisins(octet numCase);
/// Liste les cases des voisins d'une case donnée
void listerVoisins(octet numCase);
/// La position initiale de tous les pions du plateau de jeux
T_Position getPositionInitiale();
/// Affiche la position initiale de tous les pions du plateau
void afficherPosition(T_Position p);
/// Ajouter un coup à la liste des coups
void addCoup(T_ListeCoups *pL, octet origine, octet destination);
/// Afficher la liste des coups autorisés (origine et destination)
void afficherListeCoups(T_ListeCoups l);
/// Jouer un coup en mettant à jour la position des pions du plateau du jeux
T_Position jouerCoup(T_Position p, octet origine, octet destination) ;
/// Retourne l'ensemble des coups légaux pour une position des pions sur le plateau de jeux
T_ListeCoups getCoupsLegaux(T_Position p) ;
/// Evaluer le nouveau score des deux couleurs selon un plateau de jeux donné
T_Score evaluerScore(T_Position p);
/// Afficher le score pour les Jaunes puis les Rouges
void afficherScore(T_Score s);
/// Vérification de la légalité d'un coup
octet estValide(T_Position p, octet origine, octet destination);

// Gestions des erreurs
///
void throwInput();
///
void throwFile(char*);
///
void throw(char*);