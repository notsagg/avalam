# Avalam: diag.exe
Diag.exe permet l'entrée d'une position d'un plateau de jeux de type "FEN". Une fois l'exécutable lancée, diag.exe crée un fichier json contenant la position des pions sur le plateau de jeux dans une synthaxe lisible par avalam-diag.html.

Diag.exe permet également l'entrée d'un numéro de diagramme au moment de l'exécution du programme ainsi que d'une description optionnelle.

Un nom de fichier json par défaut sera prévu si l'utilisateur ne souhaite pas explicitement nommer ce dernier. Par défaut le fichier de sortie json est généré dans ../build/web/data pour faciliter son emploi par avalam-diag.html

En exécution dynamique, ce programme nécessite les librairies libavalam.so ainsi que libcjson.so, veuillez donc mettre à jour votre variable d'environnement
LD_LIBRARY_PATH comme ceci:

`LD_LIBRARY_PATH=../build/lib:../build/lib/libavalam.so:../build/lib/libcjson.so`

Le mode debug intégré à l'interface décrit le tournant du jeux en imprimant à la console:
1. Le trait
2. Le numéro du diagramme
3. La FEN

## Cahier des charges
- [ ] Passer en ligne de commande un numéro de diagramme (à l'exécution)
- [ ] Passer en ligne de commande une position de type "FEN" (à l'exécution)
- [ ] Saisie d'un nom de fichier à produire (une fois l'exécutable lancé)
- [ ] Produit un fichier json avec la position et description utilisable par avalam-diag.html
