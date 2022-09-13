/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _ASM_GENERIC_BITOPS_H_
#define _ASM_GENERIC_BITOPS_H_

#include <limits.h>
#include <bits.h>
#include <instrumented.h>
#include <asm/atomic.h>

#ifndef __ASSEMBLY__

#ifndef arch_bit_clr
static inline void arch_bit_clr(volatile unsigned long *addr, long bit)
{
    addr += BITS_WORD(bit);
    *addr &= ~BIT(bit);
}
#endif

#ifndef arch_bit_set
static inline void arch_bit_set(volatile unsigned long *addr, long bit)
{
    addr += BITS_WORD(bit);
    *addr |= BIT(bit);
}
#endif

#ifndef arch_bit_flip
static inline void arch_bit_flip(volatile unsigned long *addr, long bit)
{
    addr += BITS_WORD(bit);
    *addr ^= BIT(bit);
}
#endif

#ifndef arch_bit_test
static inline bool arch_bit_test(volatile unsigned long *addr, long bit)
{
    addr += BITS_WORD(bit);
    return !!(*addr & BIT(bit));
}
#endif

#ifndef arch_bit_test_clr
static inline bool arch_bit_test_clr(volatile unsigned long *addr, long bit)
{
    unsigned long old;
    addr += BITS_WORD(bit);
    old = *addr;
    *addr &= ~BIT(bit);
    return !!(old & BIT(bit));
}
#endif

#ifndef arch_bit_test_set
static inline bool arch_bit_test_set(volatile unsigned long *addr, long bit)
{
    unsigned long old;
    addr += BITS_WORD(bit);
    old = *addr;
    *addr |= BIT(bit);
    return !!(old & BIT(bit));
}
#endif

#ifndef arch_bit_test_flip
static inline bool arch_bit_test_flip(volatile unsigned long *addr, long bit)
{
    unsigned long old;
    addr += BITS_WORD(bit);
    old = *addr;
    *addr ^= BIT(bit);
    return !!(old & BIT(bit));
}
#endif

#ifndef arch_bit_atomic_clr
static inline void arch_bit_atomic_clr(volatile unsigned long *addr, long bit)
{
    addr += BITS_WORD(bit);
    arch_atomic_and((atomic_t *)addr, ~BIT(bit));
}
#endif

#ifndef arch_bit_atomic_set
static inline void arch_bit_atomic_set(volatile unsigned long *addr, long bit)
{
    addr += BITS_WORD(bit);
    arch_atomic_or((atomic_t *)addr, BIT(bit));
}
#endif

#ifndef arch_bit_atomic_flip
static inline void arch_bit_atomic_flip(volatile unsigned long *addr, long bit)
{
    addr += BITS_WORD(bit);
    arch_atomic_xor((atomic_t *)addr, BIT(bit));
}
#endif

#ifndef arch_bit_atomic_test
static inline bool arch_bit_atomic_test(volatile unsigned long *addr, long bit)
{
    addr += BITS_WORD(bit);
    return !!(arch_atomic_read((atomic_t *)addr) & BIT(bit));
}
#endif

#ifndef arch_bit_atomic_test_clr
static inline bool arch_bit_atomic_test_clr(volatile unsigned long *addr, long bit)
{
    unsigned long old;
    addr += BITS_WORD(bit);
    if ((READ_ONCE(*addr) & BIT(bit)) == 0) return 0;
    old = arch_atomic_fetch_and((atomic_t *)addr, ~BIT(bit));
    return !!(old & BIT(bit));
}
#endif

#ifndef arch_bit_atomic_test_set
static inline bool arch_bit_atomic_test_set(volatile unsigned long *addr, long bit)
{
    unsigned long old;
    addr += BITS_WORD(bit);
    if ((READ_ONCE(*addr) & BIT(bit)) != 0) return 1;
    old = arch_atomic_fetch_or((atomic_t *)addr, BIT(bit));
    return !!(old & BIT(bit));
}
#endif

#ifndef arch_bit_atomic_test_flip
static inline bool arch_bit_atomic_test_flip(volatile unsigned long *addr, long bit)
{
    unsigned long old;
    addr += BITS_WORD(bit);
    old = arch_atomic_fetch_xor((atomic_t *)addr, BIT(bit));
    return !!(old & BIT(bit));
}
#endif

#ifndef bit_clr
static inline void bit_clr(volatile unsigned long *addr, long bit)
{
    instrument_write(addr + BITS_WORD(bit), sizeof(*addr));
    arch_bit_clr(addr, bit);
}
#endif

#ifndef bit_set
static inline void bit_set(volatile unsigned long *addr, long bit)
{
    instrument_write(addr + BITS_WORD(bit), sizeof(*addr));
    arch_bit_set(addr, bit);
}
#endif

#ifndef bit_flip
static inline void bit_flip(volatile unsigned long *addr, long bit)
{
    instrument_write(addr + BITS_WORD(bit), sizeof(*addr));
    arch_bit_flip(addr, bit);
}
#endif

#ifndef bit_test
static inline bool bit_test(volatile unsigned long *addr, long bit)
{
    instrument_read(addr + BITS_WORD(bit), sizeof(*addr));
    return arch_bit_test(addr, bit);
}
#endif

#ifndef bit_test_clr
static inline bool bit_test_clr(volatile unsigned long *addr, long bit)
{
    instrument_read_write(addr + BITS_WORD(bit), sizeof(*addr));
    return arch_bit_test_clr(addr, bit);
}
#endif

#ifndef bit_test_set
static inline bool bit_test_set(volatile unsigned long *addr, long bit)
{
    instrument_read_write(addr + BITS_WORD(bit), sizeof(*addr));
    return arch_bit_test_set(addr, bit);
}
#endif

#ifndef bit_test_flip
static inline bool bit_test_flip(volatile unsigned long *addr, long bit)
{
    instrument_read_write(addr + BITS_WORD(bit), sizeof(*addr));
    return arch_bit_test_flip(addr, bit);
}
#endif

#ifndef bit_atomic_clr
static inline void bit_atomic_clr(volatile unsigned long *addr, long bit)
{
    instrument_atomic_write(addr + BITS_WORD(bit), sizeof(*addr));
    arch_bit_atomic_clr(addr, bit);
}
#endif

#ifndef bit_atomic_set
static inline void bit_atomic_set(volatile unsigned long *addr, long bit)
{
    instrument_atomic_write(addr + BITS_WORD(bit), sizeof(*addr));
    arch_bit_atomic_set(addr, bit);
}
#endif

#ifndef bit_atomic_flip
static inline void bit_atomic_flip(volatile unsigned long *addr, long bit)
{
    instrument_atomic_write(addr + BITS_WORD(bit), sizeof(*addr));
    arch_bit_atomic_flip(addr, bit);
}
#endif

#ifndef bit_atomic_test
static inline bool bit_atomic_test(volatile unsigned long *addr, long bit)
{
    instrument_read(addr + BITS_WORD(bit), sizeof(*addr));
    return arch_bit_atomic_test(addr, bit);
}
#endif

#ifndef bit_atomic_test_clr
static inline bool bit_atomic_test_clr(volatile unsigned long *addr, long bit)
{
    instrument_read_write(addr + BITS_WORD(bit), sizeof(*addr));
    return arch_bit_atomic_test_clr(addr, bit);
}
#endif

#ifndef bit_atomic_test_set
static inline bool bit_atomic_test_set(volatile unsigned long *addr, long bit)
{
    instrument_read_write(addr + BITS_WORD(bit), sizeof(*addr));
    return arch_bit_atomic_test_set(addr, bit);
}
#endif

#ifndef bit_atomic_test_flip
static inline bool bit_atomic_test_flip(volatile unsigned long *addr, long bit)
{
    instrument_read_write(addr + BITS_WORD(bit), sizeof(*addr));
    return arch_bit_atomic_test_flip(addr, bit);
}
#endif

/**
 * ffsuf - find first set bit in word (unsafe).
 * @val: The word to search.
 * Undefined if no bit exists, so code should check against 0 first..
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
 * flsuf: find last set bit in word (unsafe).
 * @val: The word to search.
 * Undefined if no set bit exists, so code should check against 0 first..
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
 * ffzuf - find first zero in word (unsafe).
 * @word: The word to search.
 * Undefined if no zero exists, so code should check against ~0UL first..
 */
#ifndef ffzuf
static inline unsigned int ffzuf(unsigned long value)
{
    return ffsuf(~value);
}
#endif

/*
 * flzuf - find last zero in word (unsafe).
 * @word: The word to search.
 * Undefined if no zero exists, so code should check against ~0UL first..
 */
#ifndef flzuf
static inline unsigned int flzuf(unsigned long value)
{
    return flsuf(~value);
}
#endif

/**
 * ffs - find first bit set.
 * @value: the word to search.
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
 * fls - find last zero in word.
 * @value: the word to search.
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
 * ffz - find first zero in word.
 * @value: The word to search.
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
 * flz - find last zero in word.
 * @value: The word to search.
 */
#ifndef flz
static inline unsigned int flz(unsigned long value)
{
    if (value == ULONG_MAX)
        return 0;
    return fls(~value);
}
#endif

/*
 * ctz - returns the number of trailing 0-bits in value.
 * @value: The word to search.
 */
#ifndef ctz
static inline unsigned int ctz(unsigned long value)
{
    if (value == ULONG_MAX)
        return 0;
    if (value == ULONG_MIN)
        return BITS_PER_LONG;
    return ffsuf(value);
}
#endif

/*
 * clz - returns the number of leading 0-bits in value.
 * @value: The word to search.
 */
#ifndef clz
static inline unsigned int clz(unsigned long value)
{
    if (value == ULONG_MAX)
        return 0;
    if (value == ULONG_MIN)
        return BITS_PER_LONG;
    return BITS_PER_LONG - 1 - flsuf(value);
}
#endif

#if BITS_PER_LONG == 32
static inline unsigned int ffsuf64(uint64_t value)
{
    uint32_t hi = value >> 32;

    if ((uint32_t)value)
        return ffsuf((uint32_t)value);

    return ffsuf(hi) + 32;
}
#else
# define ffsuf64 ffsuf
#endif

#if BITS_PER_LONG == 32
static inline unsigned int flsuf64(uint64_t value)
{
    uint32_t hi = value >> 32;

    if (hi)
        return flsuf(hi) + 32;

    return flsuf((uint32_t)value);
}
#else
# define flsuf64 flsuf
#endif

#if BITS_PER_LONG == 32
static inline unsigned int ffzuf64(uint64_t value)
{
    uint32_t hi = value >> 32;

    if ((uint32_t)value)
        return ffzuf((uint32_t)value);

    return ffzuf(hi) + 32;
}
#else
# define ffzuf64 ffzuf
#endif

#if BITS_PER_LONG == 32
static inline unsigned int flzuf64(uint64_t value)
{
    uint32_t hi = value >> 32;

    if (hi)
        return flzuf(hi) + 32;

    return flzuf((uint32_t)value);
}
#else
# define flzuf64 flzuf
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

#if BITS_PER_LONG == 32
static inline unsigned int ffz64(uint64_t value)
{
    uint32_t hi = value >> 32;

    if ((uint32_t)value)
        return ffz((uint32_t)value);

    return ffz(hi) + 32;
}
#else
# define ffz64 ffz
#endif

#if BITS_PER_LONG == 32
static inline unsigned int flz64(uint64_t value)
{
    uint32_t hi = value >> 32;

    if (hi)
        return flz(hi) + 32;

    return flz((uint32_t)value);
}
#else
# define flz64 flz
#endif

#if BITS_PER_LONG == 32
static inline unsigned int ctz64(uint64_t value)
{
    uint32_t hi = value >> 32;

    if ((uint32_t)value)
        return ctz(value);

    return ctz(hi) + 32;
}
#else
# define ctz64 ctz
#endif

#if BITS_PER_LONG == 32
static inline unsigned int clz64(uint64_t value)
{
    uint32_t hi = value >> 32;

    if (hi)
        return clz(hi);

    return clz((uint32_t)value) + 32;
}
#else
# define clz64 clz
#endif

#endif  /* __ASSEMBLY__ */
#endif  /* _ASM_GENERIC_BITOPS_H_ */
