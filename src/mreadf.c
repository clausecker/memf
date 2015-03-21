#include <string.h>

#include "memf.h"
#include "memf-internal.h"

static rwfunc rwmread;

/*
 * Read count bytes from file and then increment the place file points
 * to appropriately.
 */
static size_t
rwmread(void *file, uint8_t *buf, size_t count)
{
	uint8_t **source = (uint8_t**)file;

	memcpy(buf, *source, count);
	*source += count;

	return count;
}

/*
 * Read from source and marshall into repr.
 */
extern size_t
mreadf(const uint8_t *source, const char *fstr, void *restrict repr)
{
	struct rwfile rwf = {&rwmread, &source, READ};

	return memf(&rwf, fstr, repr);
}
