#ifndef FIDDLE_H
#define FIDDLE_H

#include <stdint.h>

/* byte-fiddling functions */

/* read/write intel/motorola 16/32/64 */
static inline uint8_t  rm8 (const uint8_t[1]);
static inline uint16_t rm16(const uint8_t[2]);
static inline uint32_t rm32(const uint8_t[4]);
static inline uint64_t rm64(const uint8_t[8]);
static inline uint8_t  ri8 (const uint8_t[1]);
static inline uint16_t ri16(const uint8_t[2]);
static inline uint32_t ri32(const uint8_t[4]);
static inline uint64_t ri64(const uint8_t[8]);

static inline void wm8 (uint8_t[1], uint8_t );
static inline void wm16(uint8_t[2], uint16_t);
static inline void wm32(uint8_t[4], uint32_t);
static inline void wm64(uint8_t[8], uint64_t);
static inline void wi8 (uint8_t[1], uint8_t );
static inline void wi16(uint8_t[2], uint16_t);
static inline void wi32(uint8_t[4], uint32_t);
static inline void wi64(uint8_t[8], uint64_t);

#define RFUNC(n, prefix, expr)						\
static inline uint##n##_t						\
prefix##n(const uint8_t a[n / 8])					\
{									\
	int i;								\
	uint##n##_t x = 0;						\
									\
	for (i = 0; i < n / 8; i++)					\
		x |= a[i] << 8 * (expr);				\
									\
	return x;							\
}

#define RMFUNC(n) RFUNC(n, rm, n / 8 - i - 1)
#define RIFUNC(n) RFUNC(n, ri, i)

#define WFUNC(n, prefix, expr)						\
static inline void							\
prefix##n(uint8_t a[n / 8], uint##n##_t x)				\
{									\
	int i;								\
									\
	for (i = 0; i < n / 8; i++)					\
		a[i] = 0xff & x >> 8 * (expr);				\
}

#define WMFUNC(n) WFUNC(n, wm, n / 8 - i - 1)
#define WIFUNC(n) WFUNC(n, wi, i)

RMFUNC( 8)	RIFUNC( 8)	WMFUNC( 8)	WIFUNC( 8)
RMFUNC(16)	RIFUNC(16)	WMFUNC(16)	WIFUNC(16)
RMFUNC(32)	RIFUNC(32)	WMFUNC(32)	WIFUNC(32)
RMFUNC(64)	RIFUNC(64)	WMFUNC(64)	WIFUNC(64)

#undef RFUNC
#undef WFUNC
#undef RMFUNC
#undef RIFUNC
#undef WMFUNC
#undef WIFUNC

/* hopefully portably alignof macro */
#define structalign(t) offsetof (struct {char pad; t var;}, var)
/* lowest value higher than idx aligned to a */
#define alignto(a, idx) (((idx) + (a) - 1) / (a) * (a))

#endif /* FIDDLE_H */
