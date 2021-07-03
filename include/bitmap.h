
#ifndef _BITMAP_H_
#define _BITMAP_H_

#include <bits.h>
#include <mm/gfp.h>


/**
* DOC: bitmap overview
*
* The available bitmap operations and their rough meaning in the
* case that the bitmap is a single unsigned long are thus:
*
* The generated code is more efficient when nbits is known at
* compile-time and at most BITS_PER_LONG.
*
* ::
*
*  bitmap_zero(dst, nbits)                     *dst = 0UL
*  bitmap_fill(dst, nbits)                     *dst = ~0UL
*  bitmap_copy(dst, src, nbits)                *dst = *src
*  bitmap_and(dst, src1, src2, nbits)          *dst = *src1 & *src2
*  bitmap_or(dst, src1, src2, nbits)           *dst = *src1 | *src2
*  bitmap_xor(dst, src1, src2, nbits)          *dst = *src1 ^ *src2
*  bitmap_andnot(dst, src1, src2, nbits)       *dst = *src1 & ~(*src2)
*  bitmap_complement(dst, src, nbits)          *dst = ~(*src)
*  bitmap_equal(src1, src2, nbits)             Are *src1 and *src2 equal?
*  bitmap_intersects(src1, src2, nbits)        Do *src1 and *src2 overlap?
*  bitmap_subset(src1, src2, nbits)            Is *src1 a subset of *src2?
*  bitmap_empty(src, nbits)                    Are all bits zero in *src?
*  bitmap_full(src, nbits)                     Are all bits set in *src?
*  bitmap_weight(src, nbits)                   Hamming Weight: number set bits
*  bitmap_set(dst, pos, nbits)                 Set specified bit area
*  bitmap_clear(dst, pos, nbits)               Clear specified bit area
*  bitmap_find_next_zero_area(buf, len, pos, n, mask)  Find bit free area
*  bitmap_find_next_zero_area_off(buf, len, pos, n, mask, mask_off)  as above
*  bitmap_next_clear_region(map, &start, &end, nbits)  Find next clear region
*  bitmap_next_set_region(map, &start, &end, nbits)  Find next set region
*  bitmap_for_each_clear_region(map, rs, re, start, end)
*  						Iterate over all clear regions
*  bitmap_for_each_set_region(map, rs, re, start, end)
*  						Iterate over all set regions
*  bitmap_shift_right(dst, src, n, nbits)      *dst = *src >> n
*  bitmap_shift_left(dst, src, n, nbits)       *dst = *src << n
*  bitmap_cut(dst, src, first, n, nbits)       Cut n bits from first, copy rest
*  bitmap_replace(dst, old, new, mask, nbits)  *dst = (*old & ~(*mask)) | (*new & *mask)
*  bitmap_remap(dst, src, old, new, nbits)     *dst = map(old, new)(src)
*  bitmap_bitremap(oldbit, old, new, nbits)    newbit = map(old, new)(oldbit)
*  bitmap_onto(dst, orig, relmap, nbits)       *dst = orig relative to relmap
*  bitmap_fold(dst, orig, sz, nbits)           dst bits = orig bits mod sz
*  bitmap_parse(buf, buflen, dst, nbits)       Parse bitmap dst from kernel buf
*  bitmap_parse_user(ubuf, ulen, dst, nbits)   Parse bitmap dst from user buf
*  bitmap_parselist(buf, dst, nbits)           Parse bitmap dst from kernel buf
*  bitmap_parselist_user(buf, dst, nbits)      Parse bitmap dst from user buf
*  bitmap_find_free_region(bitmap, bits, order)  Find and allocate bit region
*  bitmap_release_region(bitmap, pos, order)   Free specified bit region
*  bitmap_allocate_region(bitmap, pos, order)  Allocate specified bit region
*  bitmap_from_arr32(dst, buf, nbits)          Copy nbits from u32[] buf to dst
*  bitmap_to_arr32(buf, src, nbits)            Copy nbits from buf to u32[] dst
*  bitmap_get_value8(map, start)               Get 8bit value from map at start
*  bitmap_set_value8(map, value, start)        Set 8bit value to map at start
*
* Note, bitmap_zero() and bitmap_fill() operate over the region of
* unsigned longs, that is, bits behind bitmap till the unsigned long
* boundary will be zeroed or filled as well. Consider to use
* bitmap_clear() or bitmap_set() to make explicit zeroing or filling
* respectively.
*/

/**
* DOC: bitmap bitops
*
* Also the following operations in asm/bitops.h apply to bitmaps.::
*
*  set_bit(bit, addr)                  *addr |= bit
*  clear_bit(bit, addr)                *addr &= ~bit
*  change_bit(bit, addr)               *addr ^= bit
*  test_bit(bit, addr)                 Is bit set in *addr?
*  test_and_set_bit(bit, addr)         Set bit and return old value
*  test_and_clear_bit(bit, addr)       Clear bit and return old value
*  test_and_change_bit(bit, addr)      Change bit and return old value
*  find_first_zero_bit(addr, nbits)    Position first zero bit in *addr
*  find_first_bit(addr, nbits)         Position first set bit in *addr
*  find_next_zero_bit(addr, nbits, bit)
*                                      Position next zero bit in *addr >= bit
*  find_next_bit(addr, nbits, bit)     Position next set bit in *addr >= bit
*  find_next_and_bit(addr1, addr2, nbits, bit)
*                                      Same as find_next_bit, but in
*                                      (*addr1 & *addr2)
*
*/

/*
 * Allocation and deallocation of bitmap.
 * Provided in lib/bitmap.c to avoid circular dependency.
 */
extern unsigned long *bitmap_alloc(unsigned int nbits, gfp_t flags);
extern unsigned long *bitmap_zalloc(unsigned int nbits, gfp_t flags);
extern void bitmap_free(const unsigned long *bitmap);

/*
 * lib/bitmap.c provides these functions:
 */

extern int __bitmap_equal(const unsigned long *bitmap1,
			  const unsigned long *bitmap2, unsigned int nbits);
extern bool __pure __bitmap_or_equal(const unsigned long *src1,
				     const unsigned long *src2,
				     const unsigned long *src3,
				     unsigned int nbits);
extern void __bitmap_complement(unsigned long *dst, const unsigned long *src,
			unsigned int nbits);
extern void __bitmap_shift_right(unsigned long *dst, const unsigned long *src,
				unsigned int shift, unsigned int nbits);
extern void __bitmap_shift_left(unsigned long *dst, const unsigned long *src,
				unsigned int shift, unsigned int nbits);
extern void bitmap_cut(unsigned long *dst, const unsigned long *src,
		       unsigned int first, unsigned int cut,
		       unsigned int nbits);
extern int __bitmap_and(unsigned long *dst, const unsigned long *bitmap1,
			const unsigned long *bitmap2, unsigned int nbits);
extern void __bitmap_or(unsigned long *dst, const unsigned long *bitmap1,
			const unsigned long *bitmap2, unsigned int nbits);
extern void __bitmap_xor(unsigned long *dst, const unsigned long *bitmap1,
			const unsigned long *bitmap2, unsigned int nbits);
extern int __bitmap_andnot(unsigned long *dst, const unsigned long *bitmap1,
			const unsigned long *bitmap2, unsigned int nbits);
extern void __bitmap_replace(unsigned long *dst,
			const unsigned long *old, const unsigned long *new,
			const unsigned long *mask, unsigned int nbits);
extern int __bitmap_intersects(const unsigned long *bitmap1,
			const unsigned long *bitmap2, unsigned int nbits);
extern int __bitmap_subset(const unsigned long *bitmap1,
			const unsigned long *bitmap2, unsigned int nbits);
extern int __bitmap_weight(const unsigned long *bitmap, unsigned int nbits);
extern void __bitmap_set(unsigned long *map, unsigned int start, int len);
extern void __bitmap_clear(unsigned long *map, unsigned int start, int len);

extern unsigned long bitmap_find_next_zero_area_off(unsigned long *map,
						    unsigned long size,
						    unsigned long start,
						    unsigned int nr,
						    unsigned long align_mask,
						    unsigned long align_offset);

/**
 * bitmap_find_next_zero_area - find a contiguous aligned zero area
 * @map: The address to base the search on
 * @size: The bitmap size in bits
 * @start: The bitnumber to start searching at
 * @nr: The number of zeroed bits we're looking for
 * @align_mask: Alignment mask for zero area
 *
 * The @align_mask should be one less than a power of 2; the effect is that
 * the bit offset of all zero areas this function finds is multiples of that
 * power of 2. A @align_mask of 0 means no alignment is required.
 */
static inline unsigned long
bitmap_find_next_zero_area(unsigned long *map,
			   unsigned long size,
			   unsigned long start,
			   unsigned int nr,
			   unsigned long align_mask)
{
	return bitmap_find_next_zero_area_off(map, size, start, nr,
					      align_mask, 0);
}

extern int bitmap_parse(const char *buf, unsigned int buflen,
			unsigned long *dst, int nbits);
extern int bitmap_parse_user(const char __user *ubuf, unsigned int ulen,
			unsigned long *dst, int nbits);
extern int bitmap_parselist(const char *buf, unsigned long *maskp,
			int nmaskbits);
extern int bitmap_parselist_user(const char __user *ubuf, unsigned int ulen,
			unsigned long *dst, int nbits);
extern void bitmap_remap(unsigned long *dst, const unsigned long *src,
		const unsigned long *old, const unsigned long *new, unsigned int nbits);
extern int bitmap_bitremap(int oldbit,
		const unsigned long *old, const unsigned long *new, int bits);
extern void bitmap_onto(unsigned long *dst, const unsigned long *orig,
		const unsigned long *relmap, unsigned int bits);
extern void bitmap_fold(unsigned long *dst, const unsigned long *orig,
		unsigned int sz, unsigned int nbits);
extern int bitmap_find_free_region(unsigned long *bitmap, unsigned int bits, int order);
extern void bitmap_release_region(unsigned long *bitmap, unsigned int pos, int order);
extern int bitmap_allocate_region(unsigned long *bitmap, unsigned int pos, int order);

static inline void bitmap_zero(unsigned long *dst, unsigned int nbits)
{
    unsigned int len = BITS_TO_LONGS(nbits) * sizeof(unsigned long);
    memset(dst, 0, len);
}

static inline void bitmap_fill(unsigned long *dst, unsigned int nbits)
{
    unsigned int len = BITS_TO_LONGS(nbits) * sizeof(unsigned long);
    memset(dst, 0xff, len);
}

static inline void bitmap_copy(unsigned long *dst, const unsigned long *src, unsigned int nbits)
{
    unsigned int len = BITS_TO_LONGS(nbits) * sizeof(unsigned long);
    memcpy(dst, src, len);
}


#endif
