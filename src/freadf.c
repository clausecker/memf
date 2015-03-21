#include <stddef.h>
#include <stdio.h>

#include "memf.h"
#include "memf-internal.h"

static rwfunc rwfread;

/*
 * Write count bytes from buf to file.
 */
static size_t
rwfread(void *file, uint8_t *buf, size_t count)
{

	return fread(buf, 1, count, (FILE*)file);
}

/*
 * Read binary data from a FILE* and marshall it into repr as described by fstr.
 */
extern size_t
freadf(FILE *file, const char *fstr, void *restrict repr)
{
	struct rwfile rwf = { &rwfread, file, READ };

	return memf(&rwf, fstr, repr);
}
