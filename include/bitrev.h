/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _BITREV_H_
#define _BITREV_H_

#include <types.h>

#define bitrev8_constant(value) (                   \
    (((uint8_t)(value) & (uint8_t)0x01UL) << 7) |   \
    (((uint8_t)(value) & (uint8_t)0x02UL) << 5) |   \
    (((uint8_t)(value) & (uint8_t)0x04UL) << 3) |   \
    (((uint8_t)(value) & (uint8_t)0x08UL) << 1) |   \
    (((uint8_t)(value) & (uint8_t)0x10UL) >> 1) |   \
    (((uint8_t)(value) & (uint8_t)0x20UL) >> 3) |   \
    (((uint8_t)(value) & (uint8_t)0x40UL) >> 5) |   \
    (((uint8_t)(value) & (uint8_t)0x80UL) >> 7))

#define bitrev16_constant(value) (                      \
    (((uint16_t)(value) & (uint16_t)0x0001UL) << 15) |  \
    (((uint16_t)(value) & (uint16_t)0x0002UL) << 13) |  \
    (((uint16_t)(value) & (uint16_t)0x0004UL) << 11) |  \
    (((uint16_t)(value) & (uint16_t)0x0008UL) <<  9) |  \
    (((uint16_t)(value) & (uint16_t)0x0010UL) <<  7) |  \
    (((uint16_t)(value) & (uint16_t)0x0020UL) <<  5) |  \
    (((uint16_t)(value) & (uint16_t)0x0040UL) <<  3) |  \
    (((uint16_t)(value) & (uint16_t)0x0080UL) <<  1) |  \
    (((uint16_t)(value) & (uint16_t)0x0100UL) >>  1) |  \
    (((uint16_t)(value) & (uint16_t)0x0200UL) >>  3) |  \
    (((uint16_t)(value) & (uint16_t)0x0400UL) >>  5) |  \
    (((uint16_t)(value) & (uint16_t)0x0800UL) >>  7) |  \
    (((uint16_t)(value) & (uint16_t)0x1000UL) >>  9) |  \
    (((uint16_t)(value) & (uint16_t)0x2000UL) >> 11) |  \
    (((uint16_t)(value) & (uint16_t)0x4000UL) >> 13) |  \
    (((uint16_t)(value) & (uint16_t)0x8000UL) >> 15))

#define bitrev32_constant(value) (                          \
    (((uint32_t)(value) & (uint32_t)0x00000001UL) << 31) |  \
    (((uint32_t)(value) & (uint32_t)0x00000002UL) << 29) |  \
    (((uint32_t)(value) & (uint32_t)0x00000004UL) << 27) |  \
    (((uint32_t)(value) & (uint32_t)0x00000008UL) << 25) |  \
    (((uint32_t)(value) & (uint32_t)0x00000010UL) << 23) |  \
    (((uint32_t)(value) & (uint32_t)0x00000020UL) << 21) |  \
    (((uint32_t)(value) & (uint32_t)0x00000040UL) << 19) |  \
    (((uint32_t)(value) & (uint32_t)0x00000080UL) << 17) |  \
    (((uint32_t)(value) & (uint32_t)0x00000100UL) << 15) |  \
    (((uint32_t)(value) & (uint32_t)0x00000200UL) << 13) |  \
    (((uint32_t)(value) & (uint32_t)0x00000400UL) << 11) |  \
    (((uint32_t)(value) & (uint32_t)0x00000800UL) <<  9) |  \
    (((uint32_t)(value) & (uint32_t)0x00001000UL) <<  7) |  \
    (((uint32_t)(value) & (uint32_t)0x00002000UL) <<  5) |  \
    (((uint32_t)(value) & (uint32_t)0x00004000UL) <<  3) |  \
    (((uint32_t)(value) & (uint32_t)0x00008000UL) <<  1) |  \
    (((uint32_t)(value) & (uint32_t)0x00010000UL) >>  1) |  \
    (((uint32_t)(value) & (uint32_t)0x00020000UL) >>  3) |  \
    (((uint32_t)(value) & (uint32_t)0x00040000UL) >>  5) |  \
    (((uint32_t)(value) & (uint32_t)0x00080000UL) >>  7) |  \
    (((uint32_t)(value) & (uint32_t)0x00100000UL) >>  9) |  \
    (((uint32_t)(value) & (uint32_t)0x00200000UL) >> 11) |  \
    (((uint32_t)(value) & (uint32_t)0x00400000UL) >> 13) |  \
    (((uint32_t)(value) & (uint32_t)0x00800000UL) >> 15) |  \
    (((uint32_t)(value) & (uint32_t)0x01000000UL) >> 17) |  \
    (((uint32_t)(value) & (uint32_t)0x02000000UL) >> 19) |  \
    (((uint32_t)(value) & (uint32_t)0x04000000UL) >> 21) |  \
    (((uint32_t)(value) & (uint32_t)0x08000000UL) >> 25) |  \
    (((uint32_t)(value) & (uint32_t)0x10000000UL) >> 23) |  \
    (((uint32_t)(value) & (uint32_t)0x20000000UL) >> 27) |  \
    (((uint32_t)(value) & (uint32_t)0x40000000UL) >> 29) |  \
    (((uint32_t)(value) & (uint32_t)0x80000000UL) >> 31))

#define bitrev64_constant(value) (                                  \
    (((uint64_t)(value) & (uint64_t)0x0000000000000001ULL) << 63) | \
    (((uint64_t)(value) & (uint64_t)0x0000000000000002ULL) << 61) | \
    (((uint64_t)(value) & (uint64_t)0x0000000000000004ULL) << 59) | \
    (((uint64_t)(value) & (uint64_t)0x0000000000000008ULL) << 57) | \
    (((uint64_t)(value) & (uint64_t)0x0000000000000010ULL) << 55) | \
    (((uint64_t)(value) & (uint64_t)0x0000000000000020ULL) << 53) | \
    (((uint64_t)(value) & (uint64_t)0x0000000000000040ULL) << 51) | \
    (((uint64_t)(value) & (uint64_t)0x0000000000000080ULL) << 49) | \
    (((uint64_t)(value) & (uint64_t)0x0000000000000100ULL) << 47) | \
    (((uint64_t)(value) & (uint64_t)0x0000000000000200ULL) << 45) | \
    (((uint64_t)(value) & (uint64_t)0x0000000000000400ULL) << 43) | \
    (((uint64_t)(value) & (uint64_t)0x0000000000000800ULL) << 41) | \
    (((uint64_t)(value) & (uint64_t)0x0000000000001000ULL) << 39) | \
    (((uint64_t)(value) & (uint64_t)0x0000000000002000ULL) << 37) | \
    (((uint64_t)(value) & (uint64_t)0x0000000000004000ULL) << 35) | \
    (((uint64_t)(value) & (uint64_t)0x0000000000008000ULL) << 33) | \
    (((uint64_t)(value) & (uint64_t)0x0000000000010000ULL) << 31) | \
    (((uint64_t)(value) & (uint64_t)0x0000000000020000ULL) << 29) | \
    (((uint64_t)(value) & (uint64_t)0x0000000000040000ULL) << 27) | \
    (((uint64_t)(value) & (uint64_t)0x0000000000080000ULL) << 25) | \
    (((uint64_t)(value) & (uint64_t)0x0000000000100000ULL) << 23) | \
    (((uint64_t)(value) & (uint64_t)0x0000000000200000ULL) << 21) | \
    (((uint64_t)(value) & (uint64_t)0x0000000000400000ULL) << 19) | \
    (((uint64_t)(value) & (uint64_t)0x0000000000800000ULL) << 17) | \
    (((uint64_t)(value) & (uint64_t)0x0000000001000000ULL) << 15) | \
    (((uint64_t)(value) & (uint64_t)0x0000000002000000ULL) << 13) | \
    (((uint64_t)(value) & (uint64_t)0x0000000004000000ULL) << 11) | \
    (((uint64_t)(value) & (uint64_t)0x0000000008000000ULL) <<  9) | \
    (((uint64_t)(value) & (uint64_t)0x0000000010000000ULL) <<  7) | \
    (((uint64_t)(value) & (uint64_t)0x0000000020000000ULL) <<  5) | \
    (((uint64_t)(value) & (uint64_t)0x0000000040000000ULL) <<  3) | \
    (((uint64_t)(value) & (uint64_t)0x0000000080000000ULL) <<  1) | \
    (((uint64_t)(value) & (uint64_t)0x0000000100000000ULL) >>  1) | \
    (((uint64_t)(value) & (uint64_t)0x0000000200000000ULL) >>  3) | \
    (((uint64_t)(value) & (uint64_t)0x0000000400000000ULL) >>  5) | \
    (((uint64_t)(value) & (uint64_t)0x0000000800000000ULL) >>  7) | \
    (((uint64_t)(value) & (uint64_t)0x0000001000000000ULL) >>  9) | \
    (((uint64_t)(value) & (uint64_t)0x0000002000000000ULL) >> 11) | \
    (((uint64_t)(value) & (uint64_t)0x0000004000000000ULL) >> 13) | \
    (((uint64_t)(value) & (uint64_t)0x0000008000000000ULL) >> 15) | \
    (((uint64_t)(value) & (uint64_t)0x0000010000000000ULL) >> 17) | \
    (((uint64_t)(value) & (uint64_t)0x0000020000000000ULL) >> 19) | \
    (((uint64_t)(value) & (uint64_t)0x0000040000000000ULL) >> 21) | \
    (((uint64_t)(value) & (uint64_t)0x0000080000000000ULL) >> 25) | \
    (((uint64_t)(value) & (uint64_t)0x0000100000000000ULL) >> 23) | \
    (((uint64_t)(value) & (uint64_t)0x0000200000000000ULL) >> 27) | \
    (((uint64_t)(value) & (uint64_t)0x0000400000000000ULL) >> 29) | \
    (((uint64_t)(value) & (uint64_t)0x0000800000000000ULL) >> 31) | \
    (((uint64_t)(value) & (uint64_t)0x0001000000000000ULL) >> 33) | \
    (((uint64_t)(value) & (uint64_t)0x0002000000000000ULL) >> 35) | \
    (((uint64_t)(value) & (uint64_t)0x0004000000000000ULL) >> 37) | \
    (((uint64_t)(value) & (uint64_t)0x0008000000000000ULL) >> 39) | \
    (((uint64_t)(value) & (uint64_t)0x0010000000000000ULL) >> 41) | \
    (((uint64_t)(value) & (uint64_t)0x0020000000000000ULL) >> 43) | \
    (((uint64_t)(value) & (uint64_t)0x0040000000000000ULL) >> 45) | \
    (((uint64_t)(value) & (uint64_t)0x0080000000000000ULL) >> 47) | \
    (((uint64_t)(value) & (uint64_t)0x0100000000000000ULL) >> 49) | \
    (((uint64_t)(value) & (uint64_t)0x0200000000000000ULL) >> 51) | \
    (((uint64_t)(value) & (uint64_t)0x0400000000000000ULL) >> 53) | \
    (((uint64_t)(value) & (uint64_t)0x0800000000000000ULL) >> 55) | \
    (((uint64_t)(value) & (uint64_t)0x1000000000000000ULL) >> 57) | \
    (((uint64_t)(value) & (uint64_t)0x2000000000000000ULL) >> 59) | \
    (((uint64_t)(value) & (uint64_t)0x4000000000000000ULL) >> 61) | \
    (((uint64_t)(value) & (uint64_t)0x8000000000000000ULL) >> 63))

extern uint8_t bitrev_byte_table[256];

static inline uint8_t bitrev8_dynamic(uint8_t value)
{
    return bitrev_byte_table[value];
}

static inline uint16_t bitrev16_dynamic(uint16_t value)
{
    return ((uint16_t)bitrev8_dynamic(value & 0xff) << 8) | bitrev8_dynamic(value >> 8);
}

static inline uint32_t bitrev32_dynamic(uint32_t value)
{
    return ((uint32_t)bitrev16_dynamic(value & 0xffff) << 16) | bitrev16_dynamic(value >> 16);
}

static inline uint64_t bitrev64_dynamic(uint64_t value)
{
    return ((uint64_t)bitrev32_dynamic(value & 0xffffffff) << 32) | bitrev32_dynamic(value >> 32);
}

#define bitrev8(value) ({                   \
    uint8_t __value = (uint8_t)(value);     \
    __builtin_constant_p(__value)           \
    ? bitrev8_constant(__value)             \
    : bitrev8_dynamic(__value);             \
})

#define bitrev16(value) ({                  \
    uint16_t __value = (uint16_t)(value);   \
    __builtin_constant_p(__value)           \
    ? bitrev16_constant(__value)            \
    : bitrev16_dynamic(__value);            \
})

#define bitrev32(value) ({                  \
    uint32_t __value = (uint32_t)(value);   \
    __builtin_constant_p(__value)           \
    ? bitrev32_constant(__value)            \
    : bitrev32_dynamic(__value);            \
})

#define bitrev64(value) ({                  \
    uint64_t __value = (uint64_t)(value);   \
    __builtin_constant_p(__value)           \
    ? bitrev64_constant(__value)            \
    : bitrev64_dynamic(__value);            \
})

#endif /* _BITREV_H_ */
