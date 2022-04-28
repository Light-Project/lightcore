/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _BITOPS_H_
#define _BITOPS_H_

#include <math.h>
#include <bits.h>
#include <asm/bitops.h>
#include <lightcore/swab.h>

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

extern unsigned int comp_find_first_bit(const unsigned long *block, unsigned int bits, bool swap);
extern unsigned int comp_find_last_bit(const unsigned long *block, unsigned int bits, bool swap);
extern unsigned int comp_find_first_zero(const unsigned long *block, unsigned int bits, bool swap);
extern unsigned int comp_find_last_zero(const unsigned long *block, unsigned int bits, bool swap);
extern unsigned int comp_find_next_bit(const unsigned long *addr1, const unsigned long *addr2, unsigned int bits, unsigned int start, unsigned long invert, bool swap);
extern unsigned int comp_find_prev_bit(const unsigned long *addr1, const unsigned long *addr2, unsigned int bits, unsigned int start, unsigned long invert, bool swap);

#ifndef find_first_bit
static inline unsigned int find_first_bit(const unsigned long *addr, unsigned int bits)
{
    if (small_const_nbits(bits))
        return *addr ? ffsuf(*addr & BIT_LOW_MASK(bits)) : bits;
    else
        return comp_find_first_bit(addr, bits, false);
}
#endif

#ifndef find_last_bit
static inline unsigned int find_last_bit(const unsigned long *addr, unsigned int bits)
{
    if (small_const_nbits(bits))
        return *addr ? ffsuf(*addr & BIT_LOW_MASK(bits)) : bits;
    else
        return comp_find_last_bit(addr, bits, false);
}
#endif

#ifndef find_first_zero
static inline unsigned int find_first_zero(const unsigned long *addr, unsigned int bits)
{
    if (small_const_nbits(bits))
        return *addr ? ffz(*addr | BIT_HIGH_MASK(bits)) : bits;
    else
        return comp_find_first_zero(addr, bits, false);
}
#endif

#ifndef find_last_zero
static inline unsigned int find_last_zero(const unsigned long *addr, unsigned int bits)
{
    if (small_const_nbits(bits))
        return *addr ? flz(*addr | BIT_HIGH_MASK(bits)) : bits;
    else
        return comp_find_last_zero(addr, bits, false);
}
#endif

#ifndef find_next_bit
static inline unsigned int
find_next_bit(const unsigned long *addr, unsigned int bits, unsigned int offset)
{
    if (small_const_nbits(bits)) {
        unsigned long val;

        if (unlikely(offset >= bits))
            return bits;

        val = *addr & BIT_RANGE(bits - 1, offset);
        return val ? ffsuf(val) : bits;
    }

    return comp_find_next_bit(addr, NULL, bits, offset, 0UL, false);
}
#endif

#ifndef find_prev_bit
static inline unsigned int
find_prev_bit(const unsigned long *addr, unsigned int bits, unsigned int offset)
{
    if (small_const_nbits(bits)) {
        unsigned long val;

        if (unlikely(offset >= bits))
            return bits;

        val = *addr & BIT_RANGE(bits - 1, offset);
        return val ? flsuf(val) : bits;
    }

    return comp_find_prev_bit(addr, NULL, bits, offset, 0UL, false);
}
#endif

#ifndef find_next_zero
static inline unsigned int
find_next_zero(const unsigned long *addr, unsigned int bits, unsigned int offset)
{
    if (small_const_nbits(bits)) {
        unsigned long val;

        if (unlikely(offset >= bits))
            return bits;

        val = *addr | ~BIT_RANGE(bits - 1, offset);
        return val ? ffzuf(val) : bits;
    }

    return comp_find_next_bit(addr, NULL, bits, offset, ~0UL, false);
}
#endif

#ifndef find_prev_zero
static inline unsigned int
find_prev_zero(const unsigned long *addr, unsigned int bits, unsigned int offset)
{
    if (small_const_nbits(bits)) {
        unsigned long val;

        if (unlikely(offset >= bits))
            return bits;

        val = *addr | ~BIT_RANGE(bits - 1, offset);
        return val ? flzuf(val) : bits;
    }

    return comp_find_prev_bit(addr, NULL, bits, offset, ~0UL, false);
}
#endif

#ifndef find_next_and_bit
static inline unsigned int
find_next_and_bit(const unsigned long *addr1, const unsigned long *addr2,
                  unsigned int bits, unsigned int offset)
{
    if (small_const_nbits(bits)) {
        unsigned long val;

        if (unlikely(offset >= bits))
            return bits;

        val = *addr1 & *addr2 & BIT_RANGE(bits - 1, offset);
        return val ? ffsuf(val) : bits;
    }

    return comp_find_next_bit(addr1, addr2, bits, offset, 0UL, false);
}
#endif

#ifndef find_prev_and_bit
static inline unsigned int
find_prev_and_bit(const unsigned long *addr1, const unsigned long *addr2,
                  unsigned int bits, unsigned int offset)
{
    if (small_const_nbits(bits)) {
        unsigned long val;

        if (unlikely(offset >= bits))
            return bits;

        val = *addr1 & *addr2 & BIT_RANGE(bits - 1, offset);
        return val ? flsuf(val) : bits;
    }

    return comp_find_prev_bit(addr1, addr2, bits, offset, 0UL, false);
}
#endif

#define GENERIC_BIT_FIND_BYTEORDER_NONE_OPS(name)                                           \
static inline unsigned int                                                                  \
find_first_bit_##name(const void *addr, unsigned int bits)                                  \
{                                                                                           \
    return find_first_bit(addr, bits);                                                      \
}                                                                                           \
                                                                                            \
static inline unsigned int                                                                  \
find_last_bit_##name(const void *addr, unsigned int bits)                                   \
{                                                                                           \
    return find_last_bit(addr, bits);                                                       \
}                                                                                           \
                                                                                            \
static inline unsigned int                                                                  \
find_first_zero_##name(const void *addr, unsigned int bits)                                 \
{                                                                                           \
    return find_first_zero(addr, bits);                                                     \
}                                                                                           \
                                                                                            \
static inline unsigned int                                                                  \
find_last_zero_##name(const void *addr, unsigned int bits)                                  \
{                                                                                           \
    return find_last_zero(addr, bits);                                                      \
}                                                                                           \
                                                                                            \
static inline unsigned int                                                                  \
find_next_bit_##name(const void *addr, unsigned int bits, unsigned int offset)              \
{                                                                                           \
    return find_next_bit(addr, bits, offset);                                               \
}                                                                                           \
                                                                                            \
static inline unsigned int                                                                  \
find_prev_bit_##name(const void *addr, unsigned int bits, unsigned int offset)              \
{                                                                                           \
    return find_prev_bit(addr, bits, offset);                                               \
}                                                                                           \
                                                                                            \
static inline unsigned int                                                                  \
find_next_zero_##name(const void *addr, unsigned int bits, unsigned int offset)             \
{                                                                                           \
    return find_next_zero(addr, bits, offset);                                              \
}                                                                                           \
                                                                                            \
static inline unsigned int                                                                  \
find_prev_zero_##name(const void *addr, unsigned int bits, unsigned int offset)             \
{                                                                                           \
    return find_prev_zero(addr, bits, offset);                                              \
}                                                                                           \

#define GENERIC_BIT_FIND_BYTEORDER_SWAB_OPS(name)                                           \
static inline unsigned int                                                                  \
find_first_bit_##name(const void *addr, unsigned int bits)                                  \
{                                                                                           \
    const unsigned long *block = addr;                                                      \
    if (small_const_nbits(bits))                                                            \
        return ffs(swab(*block) & BIT_LOW_MASK(bits)) - 1;                                  \
    else                                                                                    \
        return comp_find_first_bit(addr, bits, true);                                       \
}                                                                                           \
                                                                                            \
static inline unsigned int                                                                  \
find_last_bit_##name(const void *addr, unsigned int bits)                                   \
{                                                                                           \
    const unsigned long *block = addr;                                                      \
    if (small_const_nbits(bits))                                                            \
        return fls(swab(*block) & BIT_LOW_MASK(bits)) - 1;                                  \
    else                                                                                    \
        return comp_find_last_bit(addr, bits, true);                                        \
}                                                                                           \
                                                                                            \
static inline unsigned int                                                                  \
find_first_zero_##name(const void *addr, unsigned int bits)                                 \
{                                                                                           \
    const unsigned long *block = addr;                                                      \
    if (small_const_nbits(bits))                                                            \
        return ffz(swab(*block) | BIT_HIGH_MASK(bits)) - 1;                                 \
    else                                                                                    \
        return comp_find_first_zero(addr, bits, true);                                      \
}                                                                                           \
                                                                                            \
static inline unsigned int                                                                  \
find_last_zero_##name(const void *addr, unsigned int bits)                                  \
{                                                                                           \
    const unsigned long *block = addr;                                                      \
    if (small_const_nbits(bits))                                                            \
        return flz(swab(*block) | BIT_HIGH_MASK(bits)) - 1;                                 \
    else                                                                                    \
        return comp_find_last_zero(addr, bits, true);                                       \
}                                                                                           \
                                                                                            \
static inline unsigned int                                                                  \
find_next_bit_##name(const void *addr, unsigned int bits, unsigned int offset)              \
{                                                                                           \
    const unsigned long *block = addr;                                                      \
    if (small_const_nbits(bits)) {                                                          \
        unsigned long val;                                                                  \
                                                                                            \
        if (unlikely(offset >= bits))                                                       \
            return bits;                                                                    \
                                                                                            \
        val = swab(*block) & BIT_RANGE(bits - 1, offset);                                   \
        return val ? ffsuf(val) : bits;                                                     \
    }                                                                                       \
                                                                                            \
    return comp_find_next_bit(addr, NULL, bits, offset, 0UL, true);                         \
}                                                                                           \
                                                                                            \
static inline unsigned int                                                                  \
find_prev_bit_##name(const void *addr, unsigned int bits, unsigned int offset)              \
{                                                                                           \
    const unsigned long *block = addr;                                                      \
    if (small_const_nbits(bits)) {                                                          \
        unsigned long val;                                                                  \
                                                                                            \
        if (unlikely(offset >= bits))                                                       \
            return bits;                                                                    \
                                                                                            \
        val = swab(*block) & BIT_RANGE(bits - 1, offset);                                   \
        return val ? flsuf(val) : bits;                                                     \
    }                                                                                       \
                                                                                            \
    return comp_find_prev_bit(addr, NULL, bits, offset, 0UL, true);                         \
}                                                                                           \
                                                                                            \
static inline unsigned int                                                                  \
find_next_zero_##name(const void *addr, unsigned int bits, unsigned int offset)             \
{                                                                                           \
    const unsigned long *block = addr;                                                      \
    if (small_const_nbits(bits)) {                                                          \
        unsigned long val;                                                                  \
                                                                                            \
        if (unlikely(offset >= bits))                                                       \
            return bits;                                                                    \
                                                                                            \
        val = swab(*block) | ~BIT_RANGE(bits - 1, offset);                                  \
        return val ? ffzuf(val) : bits;                                                     \
    }                                                                                       \
                                                                                            \
    return comp_find_next_bit(addr, NULL, bits, offset, ~0UL, true);                        \
}                                                                                           \
                                                                                            \
static inline unsigned int                                                                  \
find_prev_zero_##name(const void *addr, unsigned int bits, unsigned int offset)             \
{                                                                                           \
    const unsigned long *block = addr;                                                      \
    if (small_const_nbits(bits)) {                                                          \
        unsigned long val;                                                                  \
                                                                                            \
        if (unlikely(offset >= bits))                                                       \
            return bits;                                                                    \
                                                                                            \
        val = swab(*block) | ~BIT_RANGE(bits - 1, offset);                                  \
        return val ? flzuf(val) : bits;                                                     \
    }                                                                                       \
                                                                                            \
    return comp_find_prev_bit(addr, NULL, bits, offset, ~0UL, true);                        \
}                                                                                           \

#ifdef CONFIG_ARCH_LITTLE_ENDIAN
GENERIC_BIT_FIND_BYTEORDER_NONE_OPS(le)
GENERIC_BIT_FIND_BYTEORDER_SWAB_OPS(be)
#elif defined(CONFIG_ARCH_BIG_ENDIAN)
GENERIC_BIT_FIND_BYTEORDER_NONE_OPS(be)
GENERIC_BIT_FIND_BYTEORDER_SWAB_OPS(le)
#else
# error "unhandled endianity"
#endif

#define for_each_set_bit(bit, addr, size)                                           \
    for ((bit) = find_next_bit((addr), (size), 0); (bit) < (size);                  \
         (bit) = find_next_bit((addr), (size), (bit) + 1))

#define for_each_set_bit_from(bit, addr, size)                                      \
    for ((bit) = find_next_bit((addr), (size), (bit)); (bit) < (size);              \
         (bit) = find_next_bit((addr), (size), (bit) + 1))

#define for_each_set_bit_continue(bit, addr, size)                                  \
    for ((bit) = find_next_bit((addr), (size), (bit) + 1); (bit) < (size);          \
         (bit) = find_next_bit((addr), (size), (bit) + 1))

#define for_each_clear_bit(bit, addr, size)                                         \
    for ((bit) = find_next_zero((addr), (size), 0); (bit) < (size);                 \
         (bit) = find_next_zero((addr), (size), (bit) + 1))

#define for_each_clear_bit_from(bit, addr, size)                                    \
    for ((bit) = find_next_zero((addr), (size), (bit)); (bit) < (size);             \
         (bit) = find_next_zero((addr), (size), (bit) + 1))

#define for_each_clear_bit_continue(bit, addr, size)                                \
    for ((bit) = find_next_zero((addr), (size), (bit) + 1); (bit) < (size);         \
         (bit) = find_next_zero((addr), (size), (bit) + 1))

#define for_each_set_bitrange(b, e, addr, size)                                     \
    for ((b) = find_next_bit((addr), (size), 0),                                    \
         (e) = find_next_zero((addr), (size), (b) + 1);                             \
         (b) < (size);                                                              \
         (b) = find_next_bit((addr), (size), (e) + 1),                              \
         (e) = find_next_zero((addr), (size), (b) + 1))

#define for_each_set_bitrange_from(b, e, addr, size)                                \
    for (; (b) < (size);                                                            \
         (b) = find_next_bit((addr), (size), (e) + 1),                              \
         (e) = find_next_zero((addr), (size), (b) + 1))

#define for_each_set_bitrange_continue(b, e, addr, size)                            \
    for ((b) = find_next_bit((addr), (size), (b)),                                  \
         (e) = find_next_zero((addr), (size), (b) + 1);                             \
         (b) < (size);                                                              \
         (b) = find_next_bit((addr), (size), (e) + 1),                              \
         (e) = find_next_zero((addr), (size), (b) + 1))

#define for_each_clear_bitrange(b, e, addr, size)                                   \
    for ((b) = find_next_zero((addr), (size), 0),                                   \
         (e) = find_next_bit((addr), (size), (b) + 1);                              \
         (b) < (size);                                                              \
         (b) = find_next_zero((addr), (size), (e) + 1),                             \
         (e) = find_next_bit((addr), (size), (b) + 1))

#define for_each_clear_bitrange_from(b, e, addr, size)                              \
    for ((b) < (size);                                                              \
         (b) = find_next_zero((addr), (size), (e) + 1),                             \
         (e) = find_next_bit((addr), (size), (b) + 1))

#define for_each_clear_bitrange_continue(b, e, addr, size)                          \
    for ((b) = find_next_zero((addr), (size), (b)),                                 \
         (e) = find_next_bit((addr), (size), (b) + 1);                              \
         (b) < (size);                                                              \
         (b) = find_next_zero((addr), (size), (e) + 1),                             \
         (e) = find_next_bit((addr), (size), (b) + 1))

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
