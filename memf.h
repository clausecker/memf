/* formatted read into / write from binary structures */

#ifndef MEMF_H
#define MEMF_H

#include <stdint.h>

#ifdef __STDC_HOSTED__
# include <stdio.h>
extern size_t freadf(FILE*, const char*, void*);
extern size_t fwritef(FILE*, const char*, const void*);
#endif

extern size_t mreadf(const uint8_t*, const char*, void*);
extern size_t mwritef(uint8_t*, const char*, const void*);

extern size_t fdwritef(int, const char*, const void*);
extern size_t fdreadf(int, const char*, void*);

extern size_t nopf(const char*);

#endif /* MEMF_H */
