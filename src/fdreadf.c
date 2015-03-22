#define _POSIX_C_SOURCE 200809L
#include <stddef.h>
#include <unistd.h>

#include "memf.h"
#include "memf-internal.h"

static rwfunc rwfdread;

/*
 * Read count bytes from file to buf.
 */
static size_t
rwfdread(void *file, uint8_t *buf, size_t count)
{
	size_t total = 0;
	ssize_t n;

	while (total < count) {
		total += n = read(*(int*)file, buf + total, count - total);
		if (n <= 0)
			return (total - n);
	}

	return total;
}

/*
 * Read binary data from fd and unmarshall it into repr as described by fstr.
 */
extern size_t
fdreadf(int fd, const char *fstr, void *restrict repr)
{
	struct rwfile rwf = { &rwfdread, &fd, READ };

	return memf(&rwf, fstr, repr);
}
