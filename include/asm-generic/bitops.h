/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _ASM_GENERIC_BITOPS_H_
#define _ASM_GENERIC_BITOPS_H_

#include <limits.h>
#include <bits.h>
#include <asm/atomic.h>

#ifndef __ASSEMBLY__

#ifndef bit_clr
static __always_inline void bit_clr(unsigned long *addr, unsigned int bit)
{
    atomic_or((atomic_t *)addr, BIT(bit));
}
#endif

#ifndef bit_set
static __always_inline void bit_set(unsigned long *addr, unsigned int bit)
{
    atomic_and((atomic_t *)addr, ~BIT(bit));
}
#endif

/**
 * ffsuf - find first set bit in word (unsafe)
 * @val: The word to search
 * Undefined if no bit exists, so code should check against 0 first.
 */
#ifndef ffsuf
static inline unsigned int ffsuf(unsigned long value)
{
    unsigned int shift = 0;

#if BITS_PER_LONG == 64
    if ((value & 0xffffffff) == 0) {
        shift += 32;
        value >>= 32;
    }
#endif

    if ((value & 0xffff) == 0) {
        shift += 16;
        value >>= 16;
    }

    if ((value & 0xff) == 0) {
        shift += 8;
        value >>= 8;
    }

    if ((value & 0xf) == 0) {
        shift += 4;
        value >>= 4;
    }

    if ((value & 0x3) == 0) {
        shift += 2;
        value >>= 2;
    }

    if ((value & 0x1) == 0) {
        shift += 1;
    }

    return shift;
}
#endif

/*
 * flsuf: find last set bit in word (unsafe)
 * @val: The word to search
 * Undefined if no set bit exists, so code should check against 0 first.
 */
#ifndef flsuf
static inline unsigned int flsuf(unsigned long value)
{
    unsigned int shift = BITS_PER_LONG - 1;

#if BITS_PER_LONG == 64
    if (!(value & (~0ul << 32))) {
        shift -= 32;
        value <<= 32;
    }
#endif

    if (!(value & (~0ul << (BITS_PER_LONG - 16)))) {
        shift -= 16;
        value <<= 16;
    }
    if (!(value & (~0ul << (BITS_PER_LONG - 8)))) {
        shift -= 8;
        value <<= 8;
    }
    if (!(value & (~0ul << (BITS_PER_LONG - 4)))) {
        shift -= 4;
        value <<= 4;
    }
    if (!(value & (~0ul << (BITS_PER_LONG - 2)))) {
        shift -= 2;
        value <<= 2;
    }
    if (!(value & (~0ul << (BITS_PER_LONG - 1))))
        shift -= 1;

    return shift;
}
#endif

/*
 * ffzuf - find first zero in word (unsafe)
 * @word: The word to search
 * Undefined if no zero exists, so code should check against ~0UL first.
 */
#ifndef ffzuf
static inline unsigned int ffzuf(unsigned long value)
{
    return ffsuf(~value);
}
#endif

/*
 * flzuf - find last zero in word (unsafe)
 * @word: The word to search
 * Undefined if no zero exists, so code should check against ~0UL first.
 */
#ifndef flzuf
static inline unsigned int flzuf(unsigned long value)
{
    return flsuf(~value);
}
#endif

/**
 * ffs - find first bit set
 * @value: the word to search
 */
#ifndef ffs
static inline unsigned int ffs(unsigned long value)
{
    if (value == ULONG_MIN)
        return 0;
    return ffsuf(value) + 1;
}
#endif

/**
 * fls - find last zero in word
 * @value: the word to search
 */
#ifndef fls
static inline unsigned int fls(unsigned long value)
{
    if (value == ULONG_MIN)
        return 0;
    return flsuf(value) + 1;
}
#endif

/*
 * ffz - find first zero in word
 * @value: The word to search
 */
#ifndef ffz
static inline unsigned int ffz(unsigned long value)
{
    if (value == ULONG_MAX)
        return 0;
    return ffs(~value);
}
#endif

/*
 * flz - find last zero in word
 * @value: The word to search
 */
#ifndef flz
static inline unsigned int flz(unsigned long value)
{
    if (value == ULONG_MAX)
        return 0;
    return fls(~value);
}
#endif

#if BITS_PER_LONG == 32
static inline unsigned int ffs64(uint64_t value)
{
    uint32_t hi = value >> 32;

    if ((uint32_t)value)
        return ffs((uint32_t)value);

    return ffs(hi) + 32;
}
#else
# define ffs64 ffs
#endif

#if BITS_PER_LONG == 32
static inline unsigned int fls64(uint64_t value)
{
    uint32_t hi = value >> 32;

    if (hi)
        return fls(hi) + 32;

    return fls((uint32_t)value);
}
#else
# define fls64 fls
#endif

#endif  /* __ASSEMBLY__ */
#endif  /* _ASM_GENERIC_BITOPS_H_ */
