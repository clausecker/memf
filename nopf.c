#include <stddef.h>

#include "memf.h"
#include "memf-internal.h"

static rwfunc rwnop;

/*
 * Just pretend to do something.
 */
static size_t
rwnop(void *file, uint8_t *buf, size_t count)
{

	(void)buf;
	(void)file;
	(void)count;

	return count;
}

/*
 * Do a dry-run: do not read or write any data but compute how much
 * data is read / written. This function can set errno to EINVAL when
 * the formatting string is malformed.
 */
extern size_t
nopf(const char *fstr)
{
	struct rwfile rwf = { &rwnop, NULL, NOP };

	return memf(&rwf, fstr, NULL);
}
