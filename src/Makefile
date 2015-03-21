LIB=memf.a
CFLAGS=-O2
CC=c99

$(LIB): $(LIB)(memf.o) $(LIB)(freadf.o) $(LIB)(fwritef.o) \
    $(LIB)(mreadf.o) $(LIB)(mwritef.o) $(LIB)(msizef.o) \
    $(LIB)(mlenf.o)

clean:
	rm -f *.o *.a

.PHONY: clean
