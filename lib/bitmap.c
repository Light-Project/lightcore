/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <kmalloc.h>
#include <limits.h>
#include <bitmap.h>
#include <export.h>
#include <asm/bitops.h>

/**
 * bitmap_find_first - find first bit in a bitmap
 * @bitmap: the bitmap to find
 * @bits: number of bits in the bitmap
 */
unsigned int bitmap_find_first(unsigned long *bitmap, unsigned int bits)
{
    unsigned int base, len = BITS_TO_U8(bits);

    for (base = 0; base < len ; base += BITS_PER_LONG) {
        if (!*bitmap)
            bitmap++;
        else
            return base + ffs(*bitmap);
    }

    return UINT_MAX;
}

/**
 * bitmap_find_last - find last bit in a bitmap
 * @bitmap: the bitmap to find
 * @bits: number of bits in the bitmap
 */
unsigned int bitmap_find_last(unsigned long *bitmap, unsigned int bits)
{
    unsigned int base, len = BITS_TO_U8(bits);
    bitmap += BITS_TO_LONG(bits) - 1;

    for (base = len; base > 0; base += BITS_PER_LONG) {
        if (!*bitmap)
            bitmap--;
        else
            return base + fls(*bitmap);
    }

    return UINT_MAX;
}

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
