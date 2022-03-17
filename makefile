LIB=./lib
SRC=./src
SRCLIB=./srclib
OBJ=./obj
INC=./includes

CC=gcc
FLAGS=-g -Wall
EXE = servidor clean
OBJECTS = $(OBJ)/lib_socketslib.o $(OBJ)/picohttpparser.o $(OBJ)/hilos.o $(OBJ)/procesar.o $(OBJ)/servidor.o
LIBS =  $(LIB)/lib_socketslib.a $(LIB)/picohttpparser.a

all:  $(OBJECTS) $(LIBS) $(EXE)

$(OBJ)/lib_socketslib.o: $(SRCLIB)/lib_socketslib.c
	$(CC) $(FLAGS) -c $(SRCLIB)/lib_socketslib.c -o $(OBJ)/lib_socketslib.o

$(OBJ)/picohttpparser.o: $(SRCLIB)/picohttpparser.c
	$(CC) $(FLAGS) -c $(SRCLIB)/picohttpparser.c -o $(OBJ)/picohttpparser.o

$(LIB)/lib_socketslib.a: $(SRCLIB)/lib_socketslib.o
	ar rcs $(LIB)/lib_socketslib.a $(OBJ)/lib_socketslib.o 

$(LIB)/picohttpparser.a: $(SRCLIB)/picohttpparser.o
	ar rcs $(LIB)/picohttpparser.a $(OBJ)/picohttpparser.o 

$(OBJ)/hilos.o: $(SRC)/hilos.c
	$(CC) $(FLAGS) -c $(SRC)/hilos.c -o $(OBJ)/hilos.o

$(OBJ)/procesar.o: $(SRC)/procesar.c
	$(CC) $(FLAGS) -c $(SRC)/procesar.c -o $(OBJ)/procesar.o

$(OBJ)/servidor.o: $(SRC)/servidor.c
	$(CC) $(FLAGS) -c $(SRC)/servidor.c -o $(OBJ)/servidor.o 

servidor: $(OBJ)/servidor.o $(LIB)/lib_socketslib.a $(OBJ)/procesar.o $(OBJ)/picohttpparser.o $(OBJ)/hilos.o
	$(CC) $(FLAGS) $^ -o $@ -g -lpthread -lconfuse

$(OBJ)/parsertest.o: $(SRC)/parsertest.c 
	$(CC) -c $(SRC)/parsertest.c -o $(OBJ)/parsertest.o

parsertest: $(OBJ)/parsertest.o $(OBJ)/picohttpparser.o $(LIB)/lib_socketslib.a 
	$(CC) $(FLAGS) $^ -o $@


clean:
	rm -f $(SRCLIB)/*.o $(SRC)/*.o

clear:
	rm -f $(OBJ)/*.o $(LIB)/*.a
