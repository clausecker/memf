LIB=libmemf.a
CFLAGS=-O2
CC=c99

$(LIB): $(LIB)(memf.o) $(LIB)(freadf.o) $(LIB)(fwritef.o) \
    $(LIB)(mreadf.o) $(LIB)(mwritef.o) $(LIB)(sizef.o) \
    $(LIB)(lenf.o) $(LIB)(fdreadf.o) $(LIB)(fdwritef.o)

clean:
	rm -f *.o *.a

.PHONY: clean
