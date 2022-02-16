/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _BITOPS_H_
#define _BITOPS_H_

#include <bits.h>
#include <asm/bitops.h>

#define BITS_TO_U8(nr)          DIV_ROUND_UP(nr, BITS_PER_U8)
#define BITS_TO_U16(nr)         DIV_ROUND_UP(nr, BITS_PER_U16)
#define BITS_TO_U32(nr)         DIV_ROUND_UP(nr, BITS_PER_U32)
#define BITS_TO_U64(nr)         DIV_ROUND_UP(nr, BITS_PER_U64)
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

#endif  /* _BITOPS_H_ */
