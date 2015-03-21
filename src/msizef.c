#include <string.h>

#include "fiddle.h"
#include "memf.h"
#include "memf-internal.h"

/*
 * Compute the size of the structure representation as described by fstr
 * If the formatting string is invalid, errno will be set to a nonzero
 * value and the value returned is invalid. You can use this function to
 * validate the formatting string. If you call mreadf(buf, fstr, &str),
 * then msizef(fstr) should not be larger than sizeof str. It might be
 * slightly smaller because of padding added to the end of str.
 */
extern size_t
msizef(const char *fstr)
{
	struct pstate pst;
	size_t ridx = 0;
	long rep;
	int size;

	memset(&pst, 0, sizeof pst);

	while (parsefstr(fstr, &pst, &size, &rep)) {
		switch (size) {
		case 0: ridx = alignto(structalign(uint8_t), ridx); break;
		case 1: ridx = alignto(structalign(uint16_t), ridx); break;
		case 2: ridx = alignto(structalign(uint32_t), ridx); break;
		case 3: ridx = alignto(structalign(uint64_t), ridx); break;
		default: continue; /* specifier 'z' */
		}

		ridx += rep << size;
	}

	return ridx;
}
