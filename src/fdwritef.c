#define _POSIX_C_SOURCE 200809L
#include <stddef.h>
#include <unistd.h>

#include "memf.h"
#include "memf-internal.h"

static rwfunc rwfdwrite;

/*
 * Write count bytes from buf to file.
 */
static size_t
rwfdwrite(void *file, uint8_t *buf, size_t count)
{
	size_t total = 0;
	ssize_t n;

	while (total < count) {
		total += n = write(*(int*)file, buf + total, count - total);
		if (n <= 0)
			return (total - n);
	}

	return total;
}

/*
 * Marshall data in repr into a portable representation and write int fd
 */
extern size_t
fdwritef(int fd, const char *fstr, const void *repr)
{
	struct rwfile rwf = { &rwfdwrite, &fd, READ };

	return memf(&rwf, fstr, (void*)repr);
}
