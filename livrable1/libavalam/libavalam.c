#include <stdio.h>
#include <stdlib.h>
#include "avalam.h"

extern T_Voisins topologie[NBCASES];
T_Position positionInitiale;

// MARK: Manipulation du jeux
/**
Le nombre de voisins autour d'une case donnée

- Paramètre numCase: le numéro de la case où l'on cherche les voisins
- Retourne: le nombre de voisins d'une case
*/
octet nbVoisins(octet numCase) {
	return topologie[numCase].nb;
}

/**
Les numéros de cases des voisins d'un numéro de cases donné

- Paramètre numCase: le numéro de la case où l'on cherche les voisins
- Retourne: les cases voisines ainsi que le le nombre de cases voisines (pions voisins)
*/
T_Voisins getVoisins(octet numCase) {
	return topologie[numCase];
}

/**
Liste les cases des voisins d'une case donnée

- Paramètre numCase: le numéro de la case où l'on cherche les voisins
*/
void listerVoisins(octet numCase) {
	T_Voisins v = getVoisins(numCase);
	for (unsigned int i = 0; i < v.nb; ++i) {
		printf("%d ",v.cases[i]);
	}
	printf("\n");
}

/**
La position initiale de tous les pions du plateau de jeux

- Retourne: un T_Position contenant le nombre de pions empilé ainsi que le trait de la colonne
*/
T_Position getPositionInitiale() {
	return positionInitiale;
}

/**
Affiche la position initiale de tous les pions du plateau sous le format: numéro pion : nombre de pions empilés : couleur du pion

- Paramètre pL: nombre de coups légaux ainsi que les positions de ceux-çi
*/
void afficherPosition(T_Position p) {
	for (unsigned int i = 0; i < NBCASES; ++i) {
		printf("%3d : %3d : %s\n", i, p.cols[i].nb, COLNAME(p.cols[i].couleur));
	}
}

/**
Ajouter un coup à la liste des coups

- Paramètre pL: nombre de coups légaux ainsi que les positions de ceux-çi
- Paramètre origine: du coup à jouer (origine du pion à déplacer)
- Paramètre destination: du coup à jouer (destination du pion à déplacer)
*/
void addCoup(T_ListeCoups *pL, octet origine, octet destination) {
	pL->coups[pL->nb].origine = origine;
	pL->coups[pL->nb].destination = destination;
	pL->nb++;
}

/**
Afficher la liste des coups autorisés (origine et destination)

- Paramètre l: nombre de coups légaux ainsi que les positions de ceux-çi
*/
void afficherListeCoups(T_ListeCoups l) {
	for (unsigned int i = 0; i < l.nb; ++i) {
		printf("%3d : %3d -> %3d\n", i, l.coups[i].origine, l.coups[i].destination);
	}
}

/**
Jouer un coup en mettant à jour la position des pions du plateau du jeux

- Paramètre p: position des pions sur le plateau de jeux
- Paramètre origine: du pion à faire déplacer (d'où)
- Paramètre destination: du pion à faire déplacer (vers où)
- Retourne: un T_Position contenant le nombre de pions empilé ainsi que le trait de la colonne
*/
T_Position jouerCoup(T_Position p, octet origine, octet destination) {
	// NB : p est une copie de la position !!

	if (!estValide(p, origine, destination)) return p;

	// joue un coup
	p.cols[destination].nb += p.cols[origine].nb;
	p.cols[destination].couleur = p.cols[origine].couleur;
	p.cols[origine].nb = VIDE;
	p.cols[origine].couleur = VIDE;

	// on inverse le trait
	p.trait = (p.trait == JAU) ? ROU : JAU;

	// renvoie la nouvelle position
	return p;
}

/**
Retourne l'ensemble des coups légaux pour une position des pions sur le plateau de jeux

- Paramètre p: position des pions sur le plateau de jeux
- Retourne: le nombre de coups légaux ainsi que les pisitions de ceux-çi

- Notes:
1.
Pour chaque case,
Si cette case contient une colonne non vide,
Pour chaque voisin de cette colonne,
Si la somme colO+colA <= 5,
Ajouter ce coup

2.
cO: case Origine
cD: case Destination
iV: indice Voisin
*/
T_ListeCoups getCoupsLegaux(T_Position p) {
	T_ListeCoups l = { 0 };
	T_Voisins v;
	int cO, cD, iV;

	for(int cO = 0; cO < NBCASES; ++cO) { // cO: case Origine
		printf1("case %d\n", cO);
		if (p.cols[cO].nb == VIDE ) { printf0("vide!\n"); continue; }

		v = getVoisins(cO);
		for (int iV = 0; iV < v.nb; ++iV) {
			cD = v.cases[iV];
			printf2("voisin %d : case %d\n",iV, cD);
			if (p.cols[cD].nb == VIDE ) { printf0("vide!\n"); continue; }
			if (p.cols[cO].nb + p.cols[cD].nb <= 5) {
				printf0("possible !\n");
				addCoup(&l, cO,cD);
			}
		}
	}
	return l;
}

/**
Evaluer le nouveau score des deux couleurs selon un plateau de jeux donné

- Paramètre p: position des pions sur le plateau de jeux
- Retourne: le score des jaunes et rouges (nombres de tours ainsi que tours de 5 pions)
*/
T_Score evaluerScore(T_Position p) {
	T_Score s = { 0 };

	for(unsigned int i = 0; i < NBCASES; ++i) {
		if (p.cols[i].nb != VIDE)  {
			if (p.cols[i].couleur == JAU) {
				s.nbJ++;
				if (p.cols[i].nb == 5) s.nbJ5++;
			} else {
				s.nbR++;
				if (p.cols[i].nb == 5) s.nbR5++;
			}
		}
	}
	return s;
}

/**
Afficher le score pour les Jaunes puis les Rouges

- Notes:
1. Le score comporte le nombre de tours jaunes ainsi que le nombre de tours à 5 pions jaunes
2. Le score comporte le nombre de tours rouges ainsi que le nombre de tours à 5 pions rouges
*/
void afficherScore(T_Score s) {
	printf("%sJ: %d (%d piles de 5)%s - %sR : %d (%d piles de 5)%s\n", CL_JAUNE, s.nbJ, s.nbJ5, CL_BLANC, CL_ROUGE, s.nbR, s.nbR5, CL_BLANC);
}

/**
Vérification de la légalité d'un coup

- Paramètre p: position des pions sur le plateau de jeux
- Paramètre origine: du pion à faire déplacer (d'où)
- Paramètre destination: du pion à faire déplacer (vers où)
- Retourne: un octet VRAI (1) ou FAUX (0)
*/
octet estValide(T_Position p, octet origine, octet destination) {
	// 1. vérification de la possibilité de déplacement d'un pion d'une origine vers une destination
		// a. vérification de l'existence d'un pion dans la colonne d'origine
	if (p.cols[origine].nb == VIDE) {
		printf("jouerCoup impossible : la colonne %d est vide ! \n", origine);
		return FAUX;
	}

		// b. vérification de l'existence d'un pion dans la colonne de destination
	if (p.cols[destination].nb == VIDE) {
		printf("jouerCoup impossible : la colonne %d est vide ! \n", destination);
		return FAUX;
	}

		// c. vérification du nombre de pions déjà existant dans la pile de destination (<= 5)
	if (p.cols[origine].nb + p.cols[destination].nb > 5) {
		printf("jouerCoup impossible : trop de jetons entre %d et %d ! \n", origine, destination);
		return FAUX;
	}

	// 2. vérification du voisinage entre la case d'origine et de destination
	T_Voisins v = getVoisins(origine);
	for (unsigned int i = 0; i < v.nb; ++i) {
		if (v.cases[i] == destination) return VRAI; // si la destination est une des cases voisines
	}

	printf("jouerCoup impossible : cases %d et %d inaccessibles! \n", origine, destination);
	return FAUX;
}