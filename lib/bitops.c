/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <bitops.h>
#include <kernel.h>
#include <export.h>
#include <lightcore/swab.h>

/**
 * comp_find_first_bit - find first bit in a region
 * @block: the block to find
 * @bits: number of bits in the block
 */
unsigned int comp_find_first_bit(const unsigned long *block, unsigned int bits)
{
    unsigned int base;

    for (base = 0; base < bits; base += BITS_PER_LONG) {
        if (*block == ULONG_MIN)
            block++;
        else
            return base + ffsuf(*block);
    }

    return bits;
}
EXPORT_SYMBOL(comp_find_first_bit);

/**
 * comp_find_last_bit - find last bit in a region
 * @block: the block to find
 * @bits: number of bits in the block
 */
unsigned int comp_find_last_bit(const unsigned long *block, unsigned int bits)
{
    if (bits) {
        unsigned long val = BIT_LOW_MASK(bits);
        unsigned long idx = (bits - 1) / BITS_PER_LONG;

        do {
            if ((block[idx] ^ ULONG_MIN) & val)
                return idx * BITS_PER_LONG + flsuf(block[idx]);
            val = ULONG_MAX;
        } while (idx--);
    }

    return bits;
}
EXPORT_SYMBOL(comp_find_last_bit);

/**
 * comp_find_first_zero - find first zero in a region
 * @block: the block to find
 * @bits: number of bits in the block
 */
unsigned int comp_find_first_zero(const unsigned long *block, unsigned int bits)
{
    unsigned int base;

    for (base = 0; base < bits; base += BITS_PER_LONG) {
        if (*block == ULONG_MAX)
            block++;
        else
            return base + ffzuf(*block);
    }

    return bits;
}
EXPORT_SYMBOL(comp_find_first_zero);

/**
 * comp_find_last_zero - find last zero in a region
 * @block: the block to find
 * @bits: number of bits in the block
 */
unsigned int comp_find_last_zero(const unsigned long *block, unsigned int bits)
{
    if (bits) {
        unsigned long val = BIT_LOW_MASK(bits);
        unsigned long idx = (bits - 1) / BITS_PER_LONG;

        do {
            if ((block[idx] ^ ULONG_MAX) & val)
                return idx * BITS_PER_LONG + flzuf(block[idx]);
            val = ULONG_MAX;
        } while (idx--);
    }

    return bits;
}
EXPORT_SYMBOL(comp_find_last_zero);

unsigned int comp_find_next_bit(const unsigned long *addr1, const unsigned long *addr2,
                                unsigned int bits, unsigned int start,
                                unsigned long invert, bool le)
{
    unsigned long tmp, mask;

    if (unlikely(start >= bits))
        return bits;

    tmp = addr1[start / BITS_PER_LONG];
    if (addr2)
        tmp &= addr2[start / BITS_PER_LONG];
    tmp ^= invert;

    mask = BIT_HIGH_MASK(start);
    if (le)
        mask = swab(mask);

    tmp &= mask;
    start = round_down(start, BITS_PER_LONG);

    while (!tmp) {
        start += BITS_PER_LONG;
        if (start >= bits)
            return bits;

        tmp = addr1[start / BITS_PER_LONG];
        if (addr2)
            tmp &= addr2[start / BITS_PER_LONG];
        tmp ^= invert;
    }

    if (le)
        tmp = swab(tmp);

    return min(start + ffsuf(tmp), bits);
}
EXPORT_SYMBOL(comp_find_next_bit);

unsigned int comp_find_prev_bit(const unsigned long *addr1, const unsigned long *addr2,
                                unsigned int bits, unsigned int start,
                                unsigned long invert, bool le)
{
    unsigned long tmp, mask;

    if (unlikely(start >= bits))
        return bits;

    tmp = addr1[start / BITS_PER_LONG];
    if (addr2)
        tmp &= addr2[start / BITS_PER_LONG];
    tmp ^= invert;

    mask = BIT_HIGH_MASK(start);
    if (le)
        mask = swab(mask);

    tmp &= mask;
    start = round_down(start, BITS_PER_LONG);

    while (!tmp) {
        start -= BITS_PER_LONG;
        if (start >= bits)
            return bits;

        tmp = addr1[start / BITS_PER_LONG];
        if (addr2)
            tmp &= addr2[start / BITS_PER_LONG];
        tmp ^= invert;
    }

    if (le)
        tmp = swab(tmp);

    return min(start + flsuf(tmp), bits);
}
EXPORT_SYMBOL(comp_find_prev_bit);
