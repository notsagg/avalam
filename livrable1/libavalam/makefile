all: install

# Pour activer le mode debugage, il faut définir __DEBUG__
debug: DEBUG=-D'__DEBUG__'
debug: install

INC=-I../include

install: libavalam.so libavalam.a libcJSON.a libcJSON.so
	mkdir -p ../build/lib
	cp libavalam.so libavalam.a ../build/lib

libavalam.so : libavalam.c
	gcc $(DEBUG) $(INC) -shared -fPIC libavalam.c -o libavalam.so

libavalam.a : libavalam.c
	gcc $(DEBUG) $(INC) -c  libavalam.c -o libavalam.o
	ar qvs libavalam.a  libavalam.o

clean:
	rm -rf  *.so *.a *.o
	rm -rf  ../build/lib/*.so ../build/lib/*.a

libcJSON.a: ../include/cJSON.c
	gcc -c ../include/cJSON.c -o ../include/cJSON.o
	ar qvs ../build/lib/libcJSON.a ../include/cJSON.o
	rm ../include/cJSON.o

libcJSON.so: ../include/cJSON.c
	gcc -shared -fPIC ../include/cJSON.c -o ../build/lib/libcJSON.so
