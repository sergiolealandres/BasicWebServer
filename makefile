LIB=./lib
SRC=./src
SRCLIB=./srclib
OBJ=./obj
INC=./includes

CC=gcc
FLAGS=-g -Wall


all: lib_socketslib.o lib_socketslib.a servidor.o servidor clean

lib_socketslib.o: $(SRCLIB)/lib_socketslib.c
	echo "HOOOLAAA"
	$(CC) -c $(SRCLIB)/lib_socketslib.c -o $(OBJ)/lib_socketslib.o

lib_socketslib.a: $(SRCLIB)/lib_socketslib.o
	ar rcs $(LIB)/lib_socketslib.a $(OBJ)/lib_socketslib.o 

servidor.o: $(SRC)/servidor.c
	$(CC) -c $(SRC)/servidor.c -o $(OBJ)/servidor.o

servidor: $(OBJ)/servidor.o $(LIB)/lib_socketslib.a
	$(CC) $^ -o $@

clean:
	rm -f $(SRCLIB)/*.o $(SRC)/*.o

clear:
	rm -f $(OBJ)/*.o $(LIB)/*.a