/* file abstraction */

#ifndef MEMF_INTERNAL_H
#define MEMF_INTERNAL_H

#include <stdint.h>

/* read / write function(file, buffer, count) */
typedef size_t rwfunc(void*, uint8_t*, size_t);

/* read / write / ignore data (for nopf) */
enum direction { READ, WRITE, NOP };

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

extern size_t memf(struct rwfile*, const char*, void*);

#endif /* MEMF_INTERNAL_H */
