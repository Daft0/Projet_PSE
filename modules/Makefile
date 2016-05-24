# lib : Fichier Makefile
#
include ../Makefile.inc

LIBRARY=	$(PSE_LIB)/libpse.a
MODULES_SRC=	$(wildcard *.c)
MODULES_OBJ=	$(patsubst %.c,%.o,$(MODULES_SRC))

all: $(LIBRARY)

$(LIBRARY): $(MODULES_OBJ)
	rm -f $(LIBRARY)
	ar rs $(LIBRARY) $(MODULES_OBJ)
	rm -f $(MODULES_OBJ)

.c.o:
	gcc -c ${CFLAGS} $<

clean :
	rm -f $(MODULES_OBJ) $(LIBRARY) *~


