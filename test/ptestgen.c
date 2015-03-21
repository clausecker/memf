/*
 * Utility to generate test cases for parsertest.c. This utility uses the parser
 * itself to generate test cases. Please make sure that the test case behaves
 * correctly before adding it.
 */

#include <errno.h>
#include <stdio.h>
#include <string.h>
#include "memf-internal.h"

extern int
main(int argc, char *argv[])
{
	struct pstate pst;
	size_t i = 0, len;
	long rep;
	int size, failed = 0;

	if (argc != 2) {
		fprintf(stderr, "Usage: %s fstr\n", argv[0]);
		return 1;
	}

	printf("\"%s\", ", argv[1]);

	/* first, try to figure out how many items fstr contains */
	memset(&pst, 0, sizeof pst);
	len = errno = 0;
	while (parsefstr(argv[1], &pst, &size, &rep))
		len++;

	if (errno != 0) {
		/* don't care about item count if an error occured */
		printf("0, %d, NULL,\n", errno);
		return 0;
	}

	printf("%zu, 0, ITEM{", len);

	/* now populate items */
	memset(&pst, 0, sizeof pst);
	while (parsefstr(argv[1], &pst, &size, &rep))
		printf("%ld, %c, %s, ", rep, "ZCHDL"[size + 1],
		    pst.bo == MOTOROLA ? "MOTOROLA" : "INTEL");

	printf("},\n");

	return 0;
}
