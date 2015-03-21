LIB=memf.a
CFLAGS=-O2
CC=c99

memf.a: memf.a(memf.o) memf.a(freadf.o) memf.a(fwritef.o) \
    memf.a(mreadf.o) memf.a(mwritef.o) memf.a(msizef.o) \
    memf.a(mlenf.o)

clean:
	rm -f *.o *.a

.PHONY: clean
