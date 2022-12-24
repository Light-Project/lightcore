/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _BITMAP_H_
#define _BITMAP_H_

#include <string.h>
#include <bitops.h>
#include <kernel.h>
#include <mm/gfp.h>

#ifdef CONFIG_ARCH_LITTLE_ENDIAN
# define BITMAP_MEM_ALIGNMENT BITS_PER_BYTE
#else
# define BITMAP_MEM_ALIGNMENT (BITS_PER_BYTE * BYTES_PER_LONG)
#endif
#define BITMAP_MEM_MASK (BITMAP_MEM_ALIGNMENT - 1)

#define DEFINE_BITMAP(name, bits) \
    unsigned long name[BITS_TO_LONG(bits)];

extern bool bitmap_comp_equal(const unsigned long *src1, const unsigned long *src2, unsigned int bits);
extern bool bitmap_comp_or_equal(const unsigned long *src1, const unsigned long *src2, const unsigned long *src3, unsigned int bits);
extern bool bitmap_comp_intersects(const unsigned long *src1, const unsigned long *src2, unsigned int bits);
extern bool bitmap_comp_and(unsigned long *dest, const unsigned long *src1, const unsigned long *src2, unsigned int bits);
extern bool bitmap_comp_andnot(unsigned long *dest, const unsigned long *src1, const unsigned long *src2, unsigned int bits);
extern void bitmap_comp_or(unsigned long *dest, const unsigned long *src1, const unsigned long *src2, unsigned int bits);
extern void bitmap_comp_xor(unsigned long *dest, const unsigned long *src1, const unsigned long *src2, unsigned int bits);
extern void bitmap_comp_complement(unsigned long *dest, const unsigned long *src, unsigned int bits);
extern void bitmap_comp_replace(unsigned long *bitmap, const unsigned long *old, const unsigned long *new, const unsigned long *mask, unsigned int bits);
extern void bitmap_comp_set(unsigned long *bitmap, unsigned int start, unsigned int bits);
extern void bitmap_comp_clr(unsigned long *bitmap, unsigned int start, unsigned int bits);

static __always_inline bool
bitmap_empty(const unsigned long *src, unsigned int bits)
{
    if (small_const_nbits(bits))
        return !(*src & BIT_LOW_MASK(bits));
    else if (__builtin_constant_p(bits & BITMAP_MEM_MASK) && align_check(bits, BITMAP_MEM_ALIGNMENT))
        return !memdiff(src, INT_MAX, bits / BITS_PER_BYTE);
    else
        return find_first_bit(src, bits) >= bits;
}

static __always_inline bool
bitmap_full(const unsigned long *src, unsigned int bits)
{
    if (small_const_nbits(bits))
        return !(~(*src) & BIT_LOW_MASK(bits));
    else if (__builtin_constant_p(bits & BITMAP_MEM_MASK) && align_check(bits, BITMAP_MEM_ALIGNMENT))
        return !memdiff(src, INT_MAX, bits / BITS_PER_BYTE);
    else
        return find_first_zero(src, bits) >= bits;
}

static __always_inline bool
bitmap_equal(const unsigned long *src1, const unsigned long *src2, unsigned int bits)
{
    if (small_const_nbits(bits))
        return !!((*src1 ^ *src2) & BIT_LOW_MASK(bits));
    else if (__builtin_constant_p(bits & BITMAP_MEM_MASK) && align_check(bits, BITMAP_MEM_ALIGNMENT))
        return memcmp(src1, src2, bits / BITS_PER_BYTE);
    else
        return bitmap_comp_equal(src1, src2, bits);
}

static __always_inline bool
bitmap_or_equal(const unsigned long *src1, const unsigned long *src2, const unsigned long *src3, unsigned int bits)
{
    if (small_const_nbits(bits))
        return !!(((*src1 | *src2) ^ *src3) & BIT_LOW_MASK(bits));
    else
        return bitmap_comp_or_equal(src1, src2, src3, bits);
}

static __always_inline bool
bitmap_intersects(const unsigned long *src1, const unsigned long *src2, unsigned int bits)
{
    if (small_const_nbits(bits))
        return !!((*src1 & *src2) & BIT_LOW_MASK(bits));
    else
        return bitmap_comp_intersects(src1, src2, bits);
}

static __always_inline bool
bitmap_and(unsigned long *dest, const unsigned long *src1, const unsigned long *src2, unsigned int bits)
{
    if (small_const_nbits(bits))
        return !!(*dest = *src1 & *src2 & BIT_LOW_MASK(bits));
    else
        return bitmap_comp_and(dest, src1, src2, bits);
}

static __always_inline bool
bitmap_andnot(unsigned long *dest, const unsigned long *src1, const unsigned long *src2, unsigned int bits)
{
    if (small_const_nbits(bits))
        return !!(*dest = *src1 & ~(*src2) & BIT_LOW_MASK(bits));
    else
        return bitmap_comp_andnot(dest, src1, src2, bits);
}

static __always_inline void
bitmap_or(unsigned long *dest, const unsigned long *src1, const unsigned long *src2, unsigned int bits)
{
    if (small_const_nbits(bits))
        *dest = *src1 | *src2;
    else
        bitmap_comp_or(dest, src1, src2, bits);
}

static __always_inline void
bitmap_xor(unsigned long *dest, const unsigned long *src1, const unsigned long *src2, unsigned int bits)
{
    if (small_const_nbits(bits))
        *dest = *src1 ^ *src2;
    else
        bitmap_comp_xor(dest, src1, src2, bits);
}

static __always_inline void
bitmap_complement(unsigned long *dest, const unsigned long *src, unsigned int bits)
{
    if (small_const_nbits(bits))
        *dest = ~(*src);
    else
        bitmap_comp_complement(dest, src, bits);
}

static __always_inline void
bitmap_set(unsigned long *bitmap, unsigned int start, unsigned int bits)
{
    if (__builtin_constant_p(bits) && bits == 1)
        bit_set(bitmap, start);
    else if (__builtin_constant_p(start & BITMAP_MEM_MASK) && align_check(start, BITMAP_MEM_ALIGNMENT) &&
             __builtin_constant_p(bits & BITMAP_MEM_MASK) && align_check(bits, BITMAP_MEM_ALIGNMENT))
        memset((char *)bitmap + start / BITS_PER_BYTE, 0xff, bits / BITS_PER_BYTE);
    else
        bitmap_comp_set(bitmap, start, bits);
}

static __always_inline void
bitmap_clr(unsigned long *bitmap, unsigned int start, unsigned int bits)
{
    if (__builtin_constant_p(bits) && bits == 1)
        bit_clr(bitmap, start);
    else if (__builtin_constant_p(start & BITMAP_MEM_MASK) && align_check(start, BITMAP_MEM_ALIGNMENT) &&
             __builtin_constant_p(bits & BITMAP_MEM_MASK) && align_check(bits, BITMAP_MEM_ALIGNMENT))
        memset((char *)bitmap + start / BITS_PER_BYTE, 0x00, bits / BITS_PER_BYTE);
    else
        bitmap_comp_clr(bitmap, start, bits);
}

static __always_inline void
bitmap_zero(unsigned long *bitmap, unsigned int bits)
{
    unsigned int len = BITS_TO_U8(bits);
    memset(bitmap, UINT8_MIN, len);
}

static __always_inline void
bitmap_fill(unsigned long *bitmap, unsigned int bits)
{
    unsigned int len = BITS_TO_U8(bits);
    memset(bitmap, UINT8_MAX, len);
}

static __always_inline void
bitmap_copy(unsigned long *dest, unsigned long *src, unsigned int bits)
{
    unsigned int len = BITS_TO_U8(bits);
    memcpy(dest, src, len);
}

extern unsigned long *bitmap_alloc(unsigned int bits, gfp_t flags);
extern unsigned long *bitmap_zalloc(unsigned int bits, gfp_t flags);
extern void bitmap_free(unsigned long *bitmap);

#endif  /* _BITMAP_H_ */
