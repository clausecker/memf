/* generic memf function */

#include <stddef.h>
#include <string.h>
#include <errno.h>

#include "memf-internal.h"
#include "fiddle.h"

/* sizeof the longest object we support, i.e. uint64_t */
enum { OBJMAX = sizeof (uint64_t) };

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
	size_t ridx = 0, objlen = 0, fidx, count, total = 0;
	int bo = 0; /* 0: motorola, 1: intel */
	uint8_t *repr = reprarg, buf[OBJMAX];

	/* go in repr to next object of type t */
#define nextobj(t) \
    {ridx += objlen; ridx = alignto(structalign(t), ridx); objlen = sizeof (t);}

	/* access field in repr as object of type t */
#define rfield(t) (*(t*)(repr + ridx))

	/* checked read / write n bytes from file into / from buffer */
#define checkrw(n) { if ((count = file->readwrite(file, buf, n)) != n)	\
	return total;							\
    else								\
	total += count; }

	/* read uintn_t depending on byte order from buf */
#define rbo(n) (bo ? ri##n(buf) : rm##n(buf))

	/* write uintn_t depending on byte order into a */
#define wbo(n, x) (bo ? wi##n(buf, x) : wm##n(buf, x))

	/* read an n bit quantity from file and store it into repr */
#define rprocess(n) checkrw(n / 8); rfield(uint##n##_t) = rbo(n);

	/* write an n bit quantity into file after reading it from repr */
#define wprocess(n) wbo(n, rfield(uint##n##_t)); checkrw(n / 8);

	/* process an n bit quantity, either reading or writing */
#define process(n)							\
    nextobj(uint##n##_t);						\
    if (file->dir == READ) {						\
	rprocess(n)							\
    } else if (file->dir == WRITE) {					\
	wprocess(n)							\
    } else /* dir == NOP */ {						\
	checkrw(n / 8)							\
    }

	for (fidx = 0; fstr[fidx] != '\0'; fidx++)
		switch (fstr[fidx]) {
		case ' ': break;
		case 'i': bo = 1; break;
		case 'm': bo = 0; break;
		case 'c': process( 8); break;
		case 'h': process(16); break;
		case 'd': process(32); break;
		case 'l': process(64); break;
		default: errno = EINVAL; return total;
		}

	return total;
}
