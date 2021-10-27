/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _BITMAP_H_
#define _BITMAP_H_

#include <types.h>
#include <string.h>
#include <bits.h>

#define DEFINE_BITMAP(name, bits) \
    unsigned long name[BITS_TO_LONG(bits)];

static __always_inline void
bitmap_zero(unsigned long *bitmap, unsigned int bits)
{
    unsigned int len = BITS_TO_U8(bits);
    memset(bitmap, 0, len);
}

static __always_inline void
bitmap_fill(unsigned long *bitmap, unsigned int bits)
{
    unsigned int len = BITS_TO_U8(bits);
    memset(bitmap, 0xff, len);
}

static __always_inline void
bitmap_copy(unsigned long *dest, unsigned long *src, unsigned int bits)
{
    unsigned int len = BITS_TO_U8(bits);
    memcpy(dest, src, len);
}

extern unsigned int bitmap_find_first(unsigned long *bitmap, unsigned int bits);
extern unsigned long *bitmap_alloc(unsigned int bits, gfp_t flags);
extern unsigned long *bitmap_zalloc(unsigned int bits, gfp_t flags);
extern void bitmap_free(unsigned long *bitmap);

#endif  /* _BITMAP_H_ */
