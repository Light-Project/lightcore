/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <kmalloc.h>
#include <limits.h>
#include <bitmap.h>
#include <export.h>
#include <asm/bitops.h>

bool bitmap_comp_equal(const unsigned long *src1, const unsigned long *src2,
                        unsigned int bits)
{
    unsigned int index, len = BITS_DIV_LONG(bits);

    for (index = 0; index < len; ++index)
        if (src1[index] != src2[index])
            return false;

    if (BITS_MOD_LONG(bits))
        if ((src1[index] ^ src2[index]) & BIT_LOW_MASK(bits))
            return false;

    return true;
}
EXPORT_SYMBOL(bitmap_comp_equal);

bool bitmap_comp_or_equal(const unsigned long *src1, const unsigned long *src2,
                        const unsigned long *src3, unsigned int bits)
{
    unsigned int index, len = BITS_DIV_LONG(bits);

    for (index = 0; index < len; ++index)
        if ((src1[index] | src2[index]) != src3[index])
            return false;

    if (BITS_MOD_LONG(bits))
        if (((src1[index] | src2[index]) ^ src3[index]) & BIT_LOW_MASK(bits))
            return false;

    return true;
}
EXPORT_SYMBOL(bitmap_comp_or_equal);

bool bitmap_comp_intersects(const unsigned long *src1, const unsigned long *src2,
                            unsigned int bits)
{
    unsigned int index, len = BITS_DIV_LONG(bits);

    for (index = 0; index < len; ++index)
        if ((src1[index] & src2[index]))
            return true;

    if (BITS_MOD_LONG(bits))
        if ((src1[index] & src2[index]) & BIT_LOW_MASK(bits))
            return true;

    return false;
}
EXPORT_SYMBOL(bitmap_comp_intersects);

bool bitmap_comp_and(unsigned long *dest, const unsigned long *src1,
                const unsigned long *src2, unsigned int bits)
{
    unsigned int index, len = BITS_DIV_LONG(bits);
    unsigned long result;

    for (index = 0; index < len; ++index)
        result |= (dest[index] = src1[index] & src2[index]);

    if (BITS_MOD_LONG(bits))
        result |= (dest[index] = src1[index] & src2[index] & BIT_LOW_MASK(bits));

    return !!result;
}
EXPORT_SYMBOL(bitmap_comp_and);

bool bitmap_comp_andnot(unsigned long *dest, const unsigned long *src1,
                const unsigned long *src2, unsigned int bits)
{
    unsigned int index, len = BITS_DIV_LONG(bits);
    unsigned long result;

    for (index = 0; index < len; ++index)
        result |= (dest[index] = src1[index] & ~src2[index]);

    if (BITS_MOD_LONG(bits))
        result |= (dest[index] = src1[index] & ~src2[index] & BIT_LOW_MASK(bits));

    return !!result;
}
EXPORT_SYMBOL(bitmap_comp_andnot);

void bitmap_comp_or(unsigned long *dest, const unsigned long *src1,
                const unsigned long *src2, unsigned int bits)
{
    unsigned int index, len = BITS_TO_LONG(bits);

    for (index = 0; index < len; ++index)
        dest[index] = src1[index] | src2[index];
}
EXPORT_SYMBOL(bitmap_comp_or);

void bitmap_comp_xor(unsigned long *dest, const unsigned long *src1,
                const unsigned long *src2, unsigned int bits)
{
    unsigned int index, len = BITS_TO_LONG(bits);

    for (index = 0; index < len; ++index)
        dest[index] = src1[index] ^ src2[index];
}
EXPORT_SYMBOL(bitmap_comp_xor);

void bitmap_comp_complement(unsigned long *dest, const unsigned long *src,
                            unsigned int bits)
{
    unsigned int index, len = BITS_TO_LONG(bits);

    for (index = 0; index < len; ++index)
        dest[index] = ~src[index];
}
EXPORT_SYMBOL(bitmap_comp_complement);

void bitmap_comp_replace(unsigned long *bitmap, const unsigned long *old,
                const unsigned long *new, const unsigned long *mask, unsigned int bits)
{
    unsigned int index, len = BITS_TO_LONG(bits);

    for (index = 0; index < len; ++index)
        bitmap[index] = (old[index] & ~mask[index]) | (old[index] & mask[index]);
}
EXPORT_SYMBOL(bitmap_comp_replace);

void bitmap_comp_set(unsigned long *bitmap, unsigned int start, unsigned int bits)
{
    unsigned int bits_to_set = BITS_PER_LONG - BITS_MOD_LONG(start);
    unsigned long mask_to_set = BIT_HIGH_MASK(start);
    unsigned long *curr = bitmap + BITS_DIV_LONG(start);
    const unsigned int size = start + bits;

    while (bits - bits_to_set) {
        *curr++ |= mask_to_set;
        bits -= bits_to_set;
        mask_to_set = ULONG_MAX;
        bits_to_set = BITS_PER_LONG;
    }

    if (bits) {
        mask_to_set &= BIT_LOW_MASK(size);
        *curr |= mask_to_set;
    }
}
EXPORT_SYMBOL(bitmap_comp_set);

void bitmap_comp_clr(unsigned long *bitmap, unsigned int start, unsigned int bits)
{
    unsigned int bits_to_clr = BITS_PER_LONG - BITS_MOD_LONG(start);
    unsigned long mask_to_clr = BIT_HIGH_MASK(start);
    unsigned long *curr = bitmap + BITS_DIV_LONG(start);
    const unsigned int size = start + bits;

    while (bits - bits_to_clr) {
        *curr++ &= ~mask_to_clr;
        bits -= bits_to_clr;
        mask_to_clr = ULONG_MAX;
        bits_to_clr = BITS_PER_LONG;
    }

    if (bits) {
        mask_to_clr &= BIT_LOW_MASK(size);
        *curr &= ~mask_to_clr;
    }
}
EXPORT_SYMBOL(bitmap_comp_clr);

/**
 * bitmap_alloc - alloc a bitmap
 * @bits: number of bits in the bitmap
 * @flags: allocate flags
 */
unsigned long *bitmap_alloc(unsigned int bits, gfp_t flags)
{
    return kmalloc_array(BITS_TO_LONG(bits), sizeof(unsigned long), flags);
}
EXPORT_SYMBOL(bitmap_alloc);

/**
 * bitmap_zalloc - alloc and zeroed a bitmap
 * @bits: number of bits in the bitmap
 * @flags: allocate flags
 */
unsigned long *bitmap_zalloc(unsigned int bits, gfp_t flags)
{
    return kmalloc_array(BITS_TO_LONG(bits), sizeof(unsigned long), flags | GFP_ZERO);
}
EXPORT_SYMBOL(bitmap_zalloc);

/**
 * bitmap_free - free a bitmap
 * @bitmap: bitmap to free
 */
void bitmap_free(unsigned long *bitmap)
{
    kfree(bitmap);
}
EXPORT_SYMBOL(bitmap_free);
