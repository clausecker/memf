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

static inline uint64_t
ri64(const uint8_t a[8])
{
	uint64_t x;

	x  = (uint64_t)a[0] <<  0;
	x |= (uint64_t)a[1] <<  8;
	x |= (uint64_t)a[2] << 16;
	x |= (uint64_t)a[3] << 24;
	x |= (uint64_t)a[4] << 32;
	x |= (uint64_t)a[5] << 40;
	x |= (uint64_t)a[6] << 48;
	x |= (uint64_t)a[7] << 56;

	return x;
}

static inline uint64_t
rm64(const uint8_t a[8])
{
	uint64_t x;

	x  = (uint64_t)a[0] << 56;
	x |= (uint64_t)a[1] << 48;
	x |= (uint64_t)a[2] << 40;
	x |= (uint64_t)a[3] << 32;
	x |= (uint64_t)a[4] << 24;
	x |= (uint64_t)a[5] << 16;
	x |= (uint64_t)a[6] <<  8;
	x |= (uint64_t)a[7] <<  0;

	return x;
}

static inline void
wi64(uint8_t a[8], uint64_t x)
{
	a[0] = x >>  0;
	a[1] = x >>  8;
	a[2] = x >> 16;
	a[3] = x >> 24;
	a[4] = x >> 32;
	a[5] = x >> 40;
	a[6] = x >> 48;
	a[7] = x >> 56;
}

static inline void
wm64(uint8_t a[8], uint64_t x)
{
	a[0] = x >> 56;
	a[1] = x >> 48;
	a[2] = x >> 40;
	a[3] = x >> 32;
	a[4] = x >> 24;
	a[5] = x >> 16;
	a[6] = x >>  8;
	a[7] = x >>  0;
}

static inline uint32_t
ri32(const uint8_t a[4])
{
	uint32_t x;

	x  = (uint32_t)a[0] <<  0;
	x |= (uint32_t)a[1] <<  8;
	x |= (uint32_t)a[2] << 16;
	x |= (uint32_t)a[3] << 24;

	return x;
}

static inline uint32_t
rm32(const uint8_t a[4])
{
	uint32_t x;

	x  = (uint32_t)a[0] << 24;
	x |= (uint32_t)a[1] << 16;
	x |= (uint32_t)a[2] <<  8;
	x |= (uint32_t)a[3] <<  0;

	return x;
}

static inline void
wi32(uint8_t a[4], uint32_t x)
{
	a[0] = x >>  0;
	a[1] = x >>  8;
	a[2] = x >> 16;
	a[3] = x >> 24;
}

static inline void
wm32(uint8_t a[4], uint32_t x)
{
	a[0] = x >> 24;
	a[1] = x >> 16;
	a[2] = x >>  8;
	a[3] = x >>  0;
}

static inline uint16_t
ri16(const uint8_t a[2])
{
	uint16_t x;

	x  = (uint16_t)a[0] <<  0;
	x |= (uint16_t)a[1] <<  8;

	return x;
}

static inline uint16_t
rm16(const uint8_t a[2])
{
	uint16_t x;

	x  = (uint16_t)a[0] <<  8;
	x |= (uint16_t)a[1] <<  0;

	return x;
}

static inline void
wi16(uint8_t a[2], uint16_t x)
{
	a[0] = x >>  0;
	a[1] = x >>  8;
}

static inline void
wm16(uint8_t a[2], uint16_t x)
{
	a[0] = x >>  8;
	a[1] = x >>  0;
}

static inline uint8_t
ri8(const uint8_t a[1])
{
	uint8_t x;

	x  = (uint8_t)a[0] <<  0;

	return x;
}

static inline uint8_t
rm8(const uint8_t a[1])
{
	uint8_t x;

	x  = (uint8_t)a[0] <<  0;

	return x;
}

static inline void
wi8(uint8_t a[1], uint8_t x)
{
	a[0] = x >>  0;
}

static inline void
wm8(uint8_t a[1], uint8_t x)
{
	a[0] = x >>  0;
}



/* hopefully portably alignof macro */
#define structalign(t) offsetof (struct {char pad; t var;}, var)
/* lowest value higher than idx aligned to a */
#define alignto(a, idx) (((idx) + (a) - 1) / (a) * (a))

#endif /* FIDDLE_H */
