/* generic memf function */

#include <errno.h>
#include <limits.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "memf-internal.h"
#include "fiddle.h"

enum {
	/*
	 * Length of the buffer for marshalling objects.  Must be larger than
	 * sizeof (uint64_t).  A buffer of this size is placed in automatic
	 * storage (i.e. on the stack); reduce size on platforms with
	 * constrainted memory.
	 */
	BUFLEN = 512,
};

static size_t	skip_data(struct rwfile*, long);
static size_t	move_data(struct rwfile*, char*, size_t*, int, long,
    enum byteorder);

/*
 * Parse the next item of fstr with respect to the parser state described in
 * pst.  Returns 0 on end or error condition, 1 otherwise.  If an error occurs
 * during parsing, errno is set to EINVAL.  After succesful parsing an item,
 * rep contains the number of times it shall be repeated and size contains the
 * number of bytes in the item (e.g. 8 for an uint64_t), or -1 for skipping.
 * Take the byte order of the next item from pst->bo.
 */
extern int
parsefstr(const char *fstr, struct pstate *pst, int *size, long *rep)
{
	char *endptr;

begin:
	*rep = 1;

rescan:
	switch (fstr[pst->fidx++]) {
	case '\0':
		if (pst->lvl != 0 || *rep != 1)
			errno = EINVAL;

		return 0;

	case ' ':	/* skip blanks */
		goto begin;

	case 'i': pst->bo = INTEL; *rep = 1; goto begin;
	case 'm': pst->bo = MOTOROLA; *rep = 1; goto begin;

	case 'z': *size = -1; return 1;
	case 'c': *size = 0; return 1;
	case 'h': *size = 1; return 1;
	case 'd': *size = 2; return 1;
	case 'l': *size = 3; return 1;

	case '1':
	case '2':
	case '3':
	case '4':
	case '5':
	case '6':
	case '7':
	case '8':
	case '9':
		*rep = strtol(fstr + pst->fidx - 1, &endptr, 10);
		if (*rep == LONG_MAX)
			return 0;	/* errno == ERANGE */

		pst->fidx = (size_t)(endptr - fstr);

		goto rescan;

	case '(':
		if (pst->lvl == STCKSIZ) {
			errno = ENOMEM;
			return 0;
		}

		pst->stck[pst->lvl].bo = pst->bo;
		pst->stck[pst->lvl].fidx = pst->fidx;
		pst->stck[pst->lvl].rep = *rep;
		pst->lvl++;
		goto begin;

	case ')':
		if (pst->lvl == 0) {
			errno = EINVAL;
			return 0;
		}

		pst->bo = pst->stck[pst->lvl - 1].bo;

		/* do the group again? */
		if (pst->stck[pst->lvl - 1].rep-- > 1)
			pst->fidx = pst->stck[pst->lvl - 1].fidx;
		else
			pst->lvl--;

		goto begin;

	default:
		errno = EINVAL;
		return 0;
	}
}

/*
 * generic core of the memf family of functions. Operates on an abstract struct
 * rwfile with operation direction (read / write) specified by dir. Returns the
 * number of bytes read / written from / to file. If an error occurs, this count
 * is short. Use nopf() to figure out what memf() should return when it runs
 * successfully.
 */
extern size_t
memf(struct rwfile *file, const char *fstr, void *restrict reprarg)
{
	struct pstate pst;
	size_t count, total = 0, ridx = 0;
	long rep, chunk;
	int size;
	char *repr = reprarg;

	memset(&pst, 0, sizeof pst);

	while (parsefstr(fstr, &pst, &size, &rep)) {
		/* directive 'z' (skip) */
		if (size < 0) {
			total += count = skip_data(file, rep);
			if (count < rep)
				return total;

			continue;
		}

		/* align ridx according to size */
		switch (size) {
		case 0: ridx = alignto(structalign(uint8_t), ridx); break;
		case 1: ridx = alignto(structalign(uint16_t), ridx); break;
		case 2: ridx = alignto(structalign(uint32_t), ridx); break;
		case 3: ridx = alignto(structalign(uint64_t), ridx); break;
		}

		/* Process read / write in chunks of BUFLEN size */
		for (; rep > 0; rep -= BUFLEN >> size) {
			chunk = rep > BUFLEN >> size ? BUFLEN >> size : rep;

			total += count =
			    move_data(file, repr, &ridx, size, chunk, pst.bo);
			if (count < chunk << size)
				return total;
		}
	}

	return total;
}

/*
 * Move rep objects of type uint(1 << size)_t between file and repr. Assume
 * rep << size < BUFLEN. Advance ridx to point just past the last byte written
 * into repr. Return byte count from / to file; byte count is less than
 * rep << size on error.
 */
static size_t
move_data(struct rwfile *file, char *repr, size_t *ridx, int size, long rep,
    enum byteorder bo)
{
	size_t count;
	long i;
	uint8_t buf[BUFLEN];

	if (file->dir == READ) {
		count = file->readwrite(file->file, buf, size << rep);
		if (count != size << rep)
			return count;

		for (i = 0; i < rep; i += 1 << size)
			switch (size) {
#define RCASE(n) *(uint##n##_t*)(repr + *ridx + i) =			\
    bo == MOTOROLA ? rm##n(buf + i) : ri##n(buf + i)
			case 0: RCASE(8); break;
			case 1: RCASE(16); break;
			case 2: RCASE(32); break;
			case 3: RCASE(64); break;
#undef RCASE
			}
	} else {
		for (i = 0; i < rep; i += 1 << size)
			switch(size) {
#define WCASE(n) bo == MOTOROLA						\
    ? wm##n(buf + i, *(uint##n##_t*)(repr + *ridx + i))			\
    : wi##n(buf + i, *(uint##n##_t*)(repr + *ridx + i))
			case 0: WCASE(8); break;
			case 1: WCASE(16); break;
			case 2: WCASE(32); break;
			case 3: WCASE(64); break;
#undef WCASE
			}

		count = file->readwrite(file->file, buf, size << rep);
	}

	*ridx += i;
	return count;
}

/*
 * Skip rep bytes of file: write 0 bytes if directions is WRITE, ignore what has
 * been read if direction is READ. Return number of bytes skipped, byte count is
 * less than rep on error.
 */
static size_t
skip_data(struct rwfile *file, long rep)
{
	size_t n, total = 0, count;
	uint8_t buf[BUFLEN];

	memset(buf, 0, rep > BUFLEN ? BUFLEN : rep);

	for (; rep > 0; rep -= BUFLEN) {
		n = rep > BUFLEN ? BUFLEN : rep;

		total += count = file->readwrite(file->file, buf, n);
		if (count < n)
			return total;
	}

	return total;
}
