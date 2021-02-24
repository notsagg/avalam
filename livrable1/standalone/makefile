all: dep install

# Pour activer le mode debugage, il faut définir __DEBUG__
debug: DEBUG=-D'__DEBUG__'
debug: dep install

dep:
	cd ../libavalam; make

install: standalone.static standalone.exe
	cp standalone.exe ../build/
	cp standalone.static ../build/

LIB=-L../build/lib
INC=-I../include

# ajout de la librairie cJSON pour faciliter l'écriture des fichier
standalone.exe:standalone.c
	gcc $(DEBUG) $(LIB) $(INC) standalone.c -o standalone.exe -lavalam -lcJSON

standalone.static:standalone.c
	gcc -static $(DEBUG) $(INC) $(LIB)  standalone.c -o standalone.static  -lavalam -lcJSON

clean:
	rm -rf standalone.exe standalone.static
	rm -rf ../build/standalone.*
	rm -rf *.js

cleanall: clean
	cd ../libavalam; make clean
