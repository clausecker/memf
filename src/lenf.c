#include <string.h>

#include "memf.h"
#include "memf-internal.h"

/*
 * Compute how long the marshalled representation as described by fstr
 * is. If the formatting string is invalid, errno will be set to a
 * nonzero value and the value returned is invalid.
 */
extern size_t
mlenf(const char *fstr)
{
	struct pstate pst;
	size_t len = 0;
	long rep;
	int size;

	memset(&pst, 0, sizeof pst);

	while (parsefstr(fstr, &pst, &size, &rep))
		len += rep << (size < 0 ? 0 : size);

	return len;
}
