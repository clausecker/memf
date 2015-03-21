#include <errno.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include "memf-internal.h"

struct item {
	long rep;
	int size;
	enum byteorder bo;
};

struct testcase {
	const char *fstr;
	size_t itemcount;
	int errval;	/* value of errno */

	const struct item *items;
};

enum sizes {
	Z = -1,
	C = 0,
	H = 1,
	D = 2,
	L = 3
};

#define ITEM (const struct item[])
const struct testcase testcases[] = {
	")",		0, EINVAL, NULL,
	"23",		0, EINVAL, NULL,
	"(",		0, EINVAL, NULL,
	"()",		0,	0, NULL,
	"1(2(3(4())))", 0,	0, NULL,
	"1(2(3(4()))",	0, EINVAL, NULL,
	"1(2(3(4()))))",0, EINVAL, NULL,
	"999999999999999999999999999999c", 0, ERANGE, NULL,
	"d",		1,	0, ITEM{1, D, MOTOROLA},
	"ic",		1,	0, ITEM{1, C, INTEL},
	"i2(cd)",	4,	0, ITEM{1, C, INTEL, 1, D, INTEL, 1, C, INTEL, 1, D, INTEL, },
	"i2((c)d)",	4,	0, ITEM{1, C, INTEL, 1, D, INTEL, 1, C, INTEL, 1, D, INTEL, },
	"i2(zmh)l",	5,	0, ITEM{1, Z, INTEL, 1, H, MOTOROLA, 1, Z, INTEL, 1, H, MOTOROLA, 1, L, INTEL, },
	"2(c3d4h)2z",	7,	0, ITEM{1, C, MOTOROLA, 3, D, MOTOROLA, 4, H, MOTOROLA, 1, C, MOTOROLA, 3, D, MOTOROLA, 4, H, MOTOROLA, 2, Z, MOTOROLA, },
	"2 3c4d 5z",	3, 	0, ITEM{3, C, MOTOROLA, 4, D, MOTOROLA, 5, Z, MOTOROLA, },
};
#undef ITEM

static int	fails(const struct testcase*);

extern int
main()
{
	size_t i;
	int fail = 0;

	printf("testing parser... ");
	fflush(stdout);

	for (i = 0; i < sizeof testcases / sizeof *testcases; i++)
		if (fails(testcases + i)) {
			if (fail == 0)
				puts("");

			fail = 1;
			printf("parser test %zu FAILED\n", i);
		}

	if (!fail)
		puts("PASS");

	return fail;
}

/* return if test c fails */
static int
fails(const struct testcase *c)
{
	struct pstate pst;
	size_t i = 0;
	long rep;
	int size, failed = 0;

	memset(&pst, 0, sizeof pst);
	errno = 0;

	for (; parsefstr(c->fstr, &pst, &size, &rep); i++)
		if (i >= c->itemcount || rep != c->items[i].rep ||
		    size != c->items[i].size || pst.bo != c->items[i].bo)
			failed = 1;

	/* ex falso quodlibet */
	if (errno != 0 && errno == c->errval)
		return 0;

	if (i != c->itemcount)
		return 1;

	return failed;
}
