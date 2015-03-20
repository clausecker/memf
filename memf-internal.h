/* file abstraction */

#ifndef MEMF_INTERNAL_H
#define MEMF_INTERNAL_H

#include <stdint.h>

/* read / write function(file, buffer, count) */
typedef size_t rwfunc(void*, uint8_t*, size_t);

/* read / write / ignore data (for nopf) */
enum direction {READ, WRITE};

/*
 * abstraction over a streaming data source / drain. The member dir
 * says whether we want to read from or write to the rwfile. The
 * function readwrite can be used to do either. It shall return the
 * number of bytes read / written; if this number is smaller than the
 * requested number of bytes, it is assumed that an error occured.
 */
struct rwfile {
	rwfunc *readwrite;
	void *file;
	enum direction dir;
};

enum byteorder {MOTOROLA, INTEL};
enum {STCKSIZ = 15}; /* number of nesting levels allowed in parsefstr */

/* state of the fstr parser; zero value is state at beginning. */
struct pstate {
	size_t fidx;
	enum byteorder bo;
	int lvl;			/* next free stack item */

	/* nesting stack for () contexts */
	struct {
		size_t fidx;		/* index into fstr */
		unsigned long rep;	/* repetitions of the next item */
		enum byteorder bo;	/* byteorder to restore */
	} stck[STCKSIZ];
};


extern size_t	memf(struct rwfile*, const char*, void*);
extern int	parsefstr(const char*, struct pstate*, int*, long*);

#endif /* MEMF_INTERNAL_H */
