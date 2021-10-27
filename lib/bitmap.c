/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <kmalloc.h>
#include <limits.h>
#include <bitmap.h>
#include <export.h>
#include <asm/bitops.h>

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

unsigned long *bitmap_alloc(unsigned int bits, gfp_t flags)
{
    return kmalloc_array(BITS_TO_LONG(bits), sizeof(unsigned long), flags);
}

unsigned long *bitmap_zalloc(unsigned int bits, gfp_t flags)
{
    return kmalloc_array(BITS_TO_LONG(bits), sizeof(unsigned long), flags | GFP_ZERO);
}

void bitmap_free(unsigned long *bitmap)
{
    kfree(bitmap);
}

EXPORT_SYMBOL(bitmap_alloc);
EXPORT_SYMBOL(bitmap_zalloc);
EXPORT_SYMBOL(bitmap_free);
