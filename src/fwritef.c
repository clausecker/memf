#include <stddef.h>
#include <stdio.h>

#include "memf.h"
#include "memf-internal.h"

static rwfunc rwfwrite;

/*
 * write count bytes from buf to file.
 */
static size_t
rwfwrite(void *file, uint8_t *buf, size_t count)
{

	return fwrite(buf, 1, count, (FILE*)file);
}

/*
 * Write binary data to a FILE* and marshall it from repr as described by fstr.
 */
extern size_t
fwritef(FILE *file, const char *fstr, const void *repr)
{
	struct rwfile rwf = { &rwfwrite, file, WRITE };

	return memf(&rwf, fstr, (void*)repr);
}
