#include <string.h>

#include "memf.h"
#include "memf-internal.h"

static rwfunc rwmwrite;

/*
 * Write count bytes into file and then increment the place file points
 * to appropriately.
 */
static size_t
rwmwrite(void *file, uint8_t *buf, size_t count)
{
	uint8_t **dest = (uint8_t**)file;

	memcpy(*dest, buf, count);
	*dest += count;

	return count;
}

/*
 * Marshall from repr into dest.
 */
extern size_t
mwritef(uint8_t *dest, const char *fstr, const void *repr)
{
	struct rwfile rwf = {&rwmwrite, &dest, WRITE};

	return memf(&rwf, fstr, (void*)repr);
}
