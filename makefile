LIB=./lib
SRC=./src
SRCLIB=./srclib
OBJ=./obj
INC=./includes

CC=gcc
FLAGS=-g -Wall
EXE = parsertest servidor clean

all: $(EXE)

lib_socketslib.o: $(SRCLIB)/lib_socketslib.c
	$(CC) -c $(SRCLIB)/lib_socketslib.c -o $(OBJ)/lib_socketslib.o

picohttpparser.o: $(SRCLIB)/picohttpparser.c
	$(CC) -c $(SRCLIB)/picohttpparser.c -o $(OBJ)/picohttpparser.o

lib_socketslib.a: $(SRCLIB)/lib_socketslib.o
	ar rcs $(LIB)/lib_socketslib.a $(OBJ)/lib_socketslib.o 

picohttpparser.a: $(SRCLIB)/picohttpparser.o
	ar rcs $(LIB)/picohttpparser.a $(OBJ)/picohttpparser.o 

hilos.o: $(SRC)/hilos.c
	$(CC) -c $(SRC)/hilos.c -o $(OBJ)/hilos.o

servidor.o: $(SRC)/servidor.c
	$(CC) -c $(SRC)/servidor.c -o $(OBJ)/servidor.o

servidor: $(OBJ)/servidor.o $(LIB)/lib_socketslib.a $(OBJ)/hilos.o
	$(CC) $^ -o $@

parsertest.o: $(SRC)/parsertest.c 
	$(CC) -c $(SRC)/parsertest.c -o $(OBJ)/parsertest.o

parsertest: $(OBJ)/parsertest.o $(OBJ)/picohttpparser.o $(LIB)/lib_socketslib.a
	$(CC) $^ -o $@


clean:
	rm -f $(SRCLIB)/*.o $(SRC)/*.o

clear:
	rm -f $(OBJ)/*.o $(LIB)/*.a