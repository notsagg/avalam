all: dep install

# Pour activer le mode debugage, il faut définir __DEBUG__
debug: DEBUG=-D'__DEBUG__'
debug: dep install

dep:
	cd ../libavalam; make

install: diag.static diag.exe
	cp diag.exe ../build/
	cp diag.static ../build/

LIB=-L../build/lib
INC=-I../include

diag.exe:diag.c
	gcc $(DEBUG) $(LIB) $(INC) diag.c -o diag.exe -lavalam -lcJSON

diag.static:diag.c
	gcc -static $(DEBUG) $(INC) $(LIB)  diag.c -o diag.static  -lavalam -lcJSON

clean:
	rm -rf diag.exe diag.static
	rm -rf ../build/diag.*
	rm -rf *.js

cleanall: clean
	cd ../libavalam; make clean
