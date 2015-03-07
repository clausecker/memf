#include <stddef.h>

#include "memf.h"
#include "memf-internal.h"

/*
 * Do a dry-run: do not read or write any data but compute how much
 * data is read / written. This function can set errno to EINVAL when
 * the formatting string is malformed.
 */
extern size_t
nopf(const char *fstr)
{
	struct rwfile rwf = {NULL, NULL, NOP};

	return memf(&rwf, fstr, NULL);
}
