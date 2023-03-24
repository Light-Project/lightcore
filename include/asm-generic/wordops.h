/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _ASM_GENERIC_WORDOPS_H_
#define _ASM_GENERIC_WORDOPS_H_

#include <macro.h>
#include <bitops.h>

#ifdef CONFIG_ARCH_BIG_ENDIAN

struct wordops {
    const unsigned long low_bits;
    const unsigned long high_bits;
};

#define WORDOPS_CONSTANTS { \
    REPEAT_BYTE(0xfe) + 1, REPEAT_BYTE(0x7f) \
}

static __always_inline unsigned int
word_find_zero(unsigned long value)
{
    unsigned int byte = 0;

#ifdef BITS_PER_LONG == 64
    if (value >> 32)
        value >>= 32;
    else
        byte = 4;
#endif
    if (value >> 16)
        value >>= 16;
    else
        byte += 2;

    return (value >> 8) ? byte : byte + 1;
}

static __always_inline bool
word_has_zero(const struct wordops *ops, unsigned long *bits, unsigned long value)
{
    unsigned long rhs = val | ops->low_bits;
    *data = rhs;
    return (val + ops->high_bits) & ~rhs;
}

static __always_inline unsigned long
word_prep_mask(const struct wordops *ops, unsigned long rhs, unsigned long value)
{
    unsigned long mask = (value & ops->low_bits) + ops->low_bits;
    return ~(mask | rhs);
}

static __always_inline unsigned long
word_create_mask(unsigned long mask)
{
    return mask;
}

static __always_inline unsigned long
word_zero_mask(unsigned long mask)
{
    return ~1UL << flsuf(mask);
}

#else /* CONFIG_ARCH_LITTLE_ENDIAN */

struct wordops {
    const unsigned long one_bits;
    const unsigned long high_bits;
};

#define WORDOPS_CONSTANTS { \
    REPEAT_BYTE(0x01), REPEAT_BYTE(0x80) \
}

#if BITS_PER_LONG == 32
static __always_inline unsigned int
word_find_zero(unsigned long value)
{
    unsigned long tmp = (0xff0001 + value) >> 23;
    return tmp & value;
}
#else
static __always_inline unsigned int
word_find_zero(unsigned long value)
{
    value *= 0x1020304050608UL;
    return value >> 56;
}
#endif

static __always_inline unsigned long
word_has_zero(const struct wordops *ops, unsigned long *bits, unsigned long value)
{
    unsigned long mask = ((value - ops->one_bits) & ~value) & ops->high_bits;
    *bits = mask;
    return mask;
}

static __always_inline unsigned long
word_prep_mask(const struct wordops *ops, unsigned long bits, unsigned long value)
{
    return bits;
}

static __always_inline unsigned long
word_create_mask(unsigned long bits)
{
    bits = (bits - 1) & ~bits;
    return bits >> 7;
}

static __always_inline unsigned long
word_zero_mask(unsigned long mask)
{
    return mask;
}

#endif /* CONFIG_ARCH_LITTLE_ENDIAN */
#endif /* _ASM_GENERIC_WORDOPS_H_ */
