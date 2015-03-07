/* generic memf function */

#include <errno.h>
#include <limits.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "memf-internal.h"
#include "fiddle.h"

enum {
	OBJMAX = sizeof (uint64_t),	/* sizeof the widest object supported */
	STCKSIZ = 16			/* maximum stack size */
};

/* nesting stack for () contexts */
struct nesting {
	size_t fidx;		/* index into fstr */
	unsigned long rep;	/* repetitions of the next item */
	unsigned int bo;	/* byte order; 0: motorola, 1: intel */
};

/*
 * generic core of the memf family of functions. Operates on an abstract struct
 * rwfile with operation direction (read / write) specified by dir. Returns the
 * number of bytes read / written from / to file. If an error occurs, this count
 * is short. Use nopf() to figure out what memf() should return when it runs
 * successfully.
 */
extern size_t
memf(struct rwfile *file, const char *fstr, void *reprarg)
{
	struct nesting stack[STCKSIZ];
	size_t ridx = 0, objlen = 0, count, total = 0, skipcount;
	int lvl = 0;	/* current nesting level */
	char *endptr;
	uint8_t *repr = reprarg, buf[OBJMAX];

	/* top of stack */
#define tos stack[lvl]

	tos.fidx = 0;
	tos.bo = 0;
	tos.rep = 0;

	/* go in repr to next object of type t */
#define nextobj(t) {							\
    ridx += objlen;							\
    ridx = alignto(structalign(t), ridx);				\
    objlen = sizeof (t);}

	/* access field in repr as object of type t */
#define rfield(t) (*(t*)(repr + ridx))

	/* checked read / write n bytes from file into / from buffer */
#define checkrw(n) {							\
    if ((count = file->readwrite(file, buf, n)) != n)			\
	return total;							\
    else								\
	total += count; }

	/* read uintn_t depending on byte order from buf */
#define rbo(n) (tos.bo ? ri##n(buf) : rm##n(buf))

	/* write uintn_t depending on byte order into a */
#define wbo(n, x) (tos.bo ? wi##n(buf, x) : wm##n(buf, x))

	/* process an n bit quantity, either reading or writing */
#define process(n)							\
    nextobj(uint##n##_t);						\
    if (file->dir == READ) {						\
	checkrw(n / 8);							\
	rfield(uint##n##_t) = rbo(n);					\
    } else if (file->dir == WRITE) {					\
	wbo(n, rfield(uint##n##_t));					\
	checkrw(n / 8);							\
    } else /* dir == NOP */ {						\
	total += n / 8;							\
    }

	for (;;) {
		switch (fstr[tos.fidx]) {
		case '\0':
			if (lvl != 0)
				errno = EINVAL;

			return total;

		case ' ': tos.fidx++; continue;

		case 'i': tos.bo = 1; break;
		case 'm': tos.bo = 0; break;

		case 'c': process( 8); break;
		case 'h': process(16); break;
		case 'd': process(32); break;
		case 'l': process(64); break;

		case 'z':
			skipcount = tos.rep >= OBJMAX ? OBJMAX : tos.rep + 1;

			if (file->dir == READ) {
				checkrw(skipcount);
			} else if (file->dir == WRITE) {
				memset(buf, 0, skipcount);
				checkrw(skipcount);
			} else
				total += skipcount = tos.rep + 1;

			tos.rep -= skipcount - 1;
			break;

		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
			tos.rep = strtoul(fstr + tos.fidx, &endptr, 10);
			if (tos.rep == ULONG_MAX)
				return total;	/* errno == ERANGE */

			tos.fidx = (size_t)(endptr - fstr);
			break;

		case '(':
			lvl++;
			if (lvl == STCKSIZ) {
				errno = ENOMEM;
				return total;
			}

			tos.bo = stack[lvl - 1].bo;
			tos.fidx = stack[lvl - 1].fidx;
			tos.rep = 0;

			break;

		case ')':
			if (lvl == 0) {
				errno = EINVAL;
				return total;
			}

			lvl--;
			if (tos.rep == 0)
				tos.fidx = stack[lvl + 1].fidx;

			break;

		default:
			errno = EINVAL;
			return total;
		}

		if (tos.rep > 0)
			tos.rep--;
		else
			tos.fidx++;
	}
}
