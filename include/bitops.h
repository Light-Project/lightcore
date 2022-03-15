/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _BITOPS_H_
#define _BITOPS_H_

#include <math.h>
#include <bits.h>
#include <asm/bitops.h>

#define BITS_DIV_U8(nr)         ((nr) / BITS_PER_U8)
#define BITS_DIV_U16(nr)        ((nr) / BITS_PER_U16)
#define BITS_DIV_U32(nr)        ((nr) / BITS_PER_U32)
#define BITS_DIV_U64(nr)        ((nr) / BITS_PER_U64)
#define BITS_DIV_CHAR(nr)       ((nr) / BITS_PER_CHAR)
#define BITS_DIV_SHORT(nr)      ((nr) / BITS_PER_SHORT)
#define BITS_DIV_INT(nr)        ((nr) / BITS_PER_INT)
#define BITS_DIV_LONG(nr)       ((nr) / BITS_PER_LONG)
#define BITS_DIV_LONG_LONG(nr)  ((nr) / BITS_PER_LONG_LONG)

#define BITS_MOD_U8(nr)         ((nr) % BITS_PER_U8)
#define BITS_MOD_U16(nr)        ((nr) % BITS_PER_U16)
#define BITS_MOD_U32(nr)        ((nr) % BITS_PER_U32)
#define BITS_MOD_U64(nr)        ((nr) % BITS_PER_U64)
#define BITS_MOD_CHAR(nr)       ((nr) % BITS_PER_CHAR)
#define BITS_MOD_SHORT(nr)      ((nr) % BITS_PER_SHORT)
#define BITS_MOD_INT(nr)        ((nr) % BITS_PER_INT)
#define BITS_MOD_LONG(nr)       ((nr) % BITS_PER_LONG)
#define BITS_MOD_LONG_LONG(nr)  ((nr) % BITS_PER_LONG_LONG)

#define BITS_TO_U8(nr)          DIV_ROUND_UP(nr, BITS_PER_U8)
#define BITS_TO_U16(nr)         DIV_ROUND_UP(nr, BITS_PER_U16)
#define BITS_TO_U32(nr)         DIV_ROUND_UP(nr, BITS_PER_U32)
#define BITS_TO_U64(nr)         DIV_ROUND_UP(nr, BITS_PER_U64)
#define BITS_TO_CHAR(nr)        DIV_ROUND_UP(nr, BITS_PER_CHAR)
#define BITS_TO_SHORT(nr)       DIV_ROUND_UP(nr, BITS_PER_SHORT)
#define BITS_TO_INT(nr)         DIV_ROUND_UP(nr, BITS_PER_INT)
#define BITS_TO_LONG(nr)        DIV_ROUND_UP(nr, BITS_PER_LONG)
#define BITS_TO_LONG_LONG(nr)   DIV_ROUND_UP(nr, BITS_PER_LONG_LONG)

/**
 * rol8 - rotate an 8-bit value left
 * @word: value to rotate
 * @shift: bits to roll
 */
static inline uint8_t rol8(uint8_t word, unsigned int shift)
{
	return (word << (shift & 7)) | (word >> ((-shift) & 7));
}

/**
 * ror8 - rotate an 8-bit value right
 * @word: value to rotate
 * @shift: bits to roll
 */
static inline uint8_t ror8(uint8_t word, unsigned int shift)
{
	return (word >> (shift & 7)) | (word << ((-shift) & 7));
}

/**
 * rol16 - rotate a 16-bit value left
 * @word: value to rotate
 * @shift: bits to roll
 */
static inline uint16_t rol16(uint16_t word, unsigned int shift)
{
	return (word << (shift & 15)) | (word >> ((-shift) & 15));
}

/**
 * ror16 - rotate a 16-bit value right
 * @word: value to rotate
 * @shift: bits to roll
 */
static inline uint16_t ror16(uint16_t word, unsigned int shift)
{
	return (word >> (shift & 15)) | (word << ((-shift) & 15));
}

/**
 * rol32 - rotate a 32-bit value left
 * @word: value to rotate
 * @shift: bits to roll
 */
static inline uint32_t rol32(uint32_t word, unsigned int shift)
{
    return (word << (shift & 31)) | (word >> ((-shift) & 31));
}

/**
 * ror32 - rotate a 32-bit value right
 * @word: value to rotate
 * @shift: bits to roll
 */
static inline uint32_t ror32(uint32_t word, unsigned int shift)
{
    return (word >> (shift & 31)) | (word << ((-shift) & 31));
}

/**
 * rol64 - rotate a 64-bit value left
 * @word: value to rotate
 * @shift: bits to roll
 */
static inline uint64_t rol64(uint64_t word, unsigned int shift)
{
    return (word << (shift & 63)) | (word >> ((-shift) & 63));
}

/**
 * ror64 - rotate a 64-bit value right
 * @word: value to rotate
 * @shift: bits to roll
 */
static inline uint64_t ror64(uint64_t word, unsigned int shift)
{
    return (word >> (shift & 63)) | (word << ((-shift) & 63));
}

extern unsigned int comp_find_first_bit(const unsigned long *block, unsigned int bits);
extern unsigned int comp_find_last_bit(const unsigned long *block, unsigned int bits);
extern unsigned int comp_find_first_zero(const unsigned long *block, unsigned int bits);
extern unsigned int comp_find_last_zero(const unsigned long *block, unsigned int bits);

static inline unsigned int find_first_bit(const unsigned long *block, unsigned int bits)
{
    if (small_const_nbits(bits))
        return ffs(*block) - 1;
    else
        return comp_find_first_bit(block, bits);
}

static inline unsigned int find_last_bit(const unsigned long *block, unsigned int bits)
{
    if (small_const_nbits(bits))
        return fls(*block) - 1;
    else
        return comp_find_last_bit(block, bits);
}

static inline unsigned int find_first_zero(const unsigned long *block, unsigned int bits)
{
    if (small_const_nbits(bits))
        return ffz(*block) - 1;
    else
        return comp_find_first_zero(block, bits);
}

static inline unsigned int find_last_zero(const unsigned long *block, unsigned int bits)
{
    if (small_const_nbits(bits))
        return flz(*block) - 1;
    else
        return comp_find_last_zero(block, bits);
}

#define GENERIC_STRUCT_BITOPS(name, type, member)                                   \
static inline void generic_##name##_##member##_set(type *ptr, unsigned int bit)     \
{                                                                                   \
    bit_set(&ptr->member, bit);                                                     \
}                                                                                   \
                                                                                    \
static inline void generic_##name##_##member##_clr(type *ptr, unsigned int bit)     \
{                                                                                   \
    bit_clr(&ptr->member, bit);                                                     \
}                                                                                   \
                                                                                    \
static inline bool generic_##name##_##member##_test(type *ptr, unsigned int bit)    \
{                                                                                   \
    return bit_test(&ptr->member, bit);                                             \
}

#endif  /* _BITOPS_H_ */
