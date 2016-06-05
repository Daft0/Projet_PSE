# PROJET : Fichier Makefile
#
include ../Makefile.inc

EXE = serveur client
SRC= $(wildcard *.c)
OBJ= $(SRC:.c=.o)

all: ${EXE}

${EXE): ${PSE_LIB}

clean:
	rm -f *.o *~ ${EXE}

