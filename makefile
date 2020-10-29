IDIR =include
CC=gcc
CFLAGS=-I$(IDIR)

ODIR=source
LDIR =../lib

LIBS=-lm

_DEPS = shared.h encoding.h decoding.h 
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJ = steganography.c shared.c encoding.c decoding.c 
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))


$(ODIR)/%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

steganography: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

.PHONY: clean

clean:
	rm -f $(ODIR)/*.o *~ core $(INCDIR)/*~ 