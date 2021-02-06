# Avalam: standalone.exe
Standalone.exe est un jeux à deux joueurs. Chaque joueur saisie à son tour la pièce à déplacer (origine, destination). A chaque mouvement, un fichier json est généré enregistrant le trait, le score des jaunes et rouges, ainsi que la position de tous les pions sur le plateau.

Le nom du fichier json généré peut être saisie au moment de l'exécution du programme sinon un nom par défaut sera employé.
Par défaut le fichier de sortie json est généré dans ../build/web pour faciliter son emploi par avalam-refresh.html

En exécution dynamique, ce programme nécessite les librairies libavalam.so ainsi que libcjson.so, veuillez donc mettre à jour votre variable d'environnement
LD_LIBRARY_PATH comme ceci:

`LD_LIBRARY_PATH=../build/lib:../build/lib/libavalam.so:../build/lib/libcjson.so`

Le mode debug intégré à l'interface décrit le tournant du jeux en imprimant à la console:
1. Le trait
2. Le score des jaunes
3. Le score des rouges
4. Le nombre de coups restants
5. La couleur ayant le score le plus élevé (en tête de jeux)

## Bugs connus
1. Boucle infini si un caractère est saisie à la place d'un entier au moment d'entrer l'origine et la destination du pion à déplacer

## Bugs résolus
1. L'écriture dans le fichier de sortie json ne se faisait pas
2. Mauvaise allocation dynamique de mémoire causait un plantage à l'appel de free()
