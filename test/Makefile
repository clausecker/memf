CC=c99
CFLAGS=-O2 -L../src/ -I../src/
TESTS=parsertest

test: parsertest
	@./parsertest

.o:
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $< -lmemf

.c:
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $< -lmemf

clean:
	rm -f *.o $(TESTS)

.PHONY: test clean
