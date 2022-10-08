/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _POPCOUNT_H_
#define _POPCOUNT_H_

#include <types.h>
#include <stddef.h>

extern const uint8_t popcount_table[256];
extern const uint8_t popparity_table[256];

static __attribute_const__ __always_inline
unsigned int popcount8_const(uint8_t value)
{
    value = (value & 0x55) + ((value >> 1) & 0x55);
    value = (value & 0x33) + ((value >> 2) & 0x33);
    value = (value & 0x0f) + ((value >> 4) & 0x0f);
    return (unsigned int)value;
}

static __attribute_const__ __always_inline
unsigned int popcount16_const(uint16_t value)
{
    value = (value & 0x5555) + ((value >> 1) & 0x5555);
    value = (value & 0x3333) + ((value >> 2) & 0x3333);
    value = (value & 0x0f0f) + ((value >> 4) & 0x0f0f);
    value = (value & 0x00ff) + ((value >> 8) & 0x00ff);
    return (unsigned int)value;
}

static __attribute_const__ __always_inline
unsigned int popcount32_const(uint32_t value)
{
    value = (value & 0x55555555UL) + ((value >>  1) & 0x55555555);
    value = (value & 0x33333333UL) + ((value >>  2) & 0x33333333);
    value = (value & 0x0f0f0f0fUL) + ((value >>  4) & 0x0f0f0f0f);
    value = (value & 0x00ff00ffUL) + ((value >>  8) & 0x00ff00ff);
    value = (value & 0x0000ffffUL) + ((value >> 16) & 0x0000ffff);
    return (unsigned int)value;
}

static __attribute_const__ __always_inline
unsigned int popcount64_const(uint64_t value)
{
    value = (value & 0x5555555555555555ULL) + ((value >>  1) & 0x5555555555555555ULL);
    value = (value & 0x3333333333333333ULL) + ((value >>  2) & 0x3333333333333333ULL);
    value = (value & 0x0f0f0f0f0f0f0f0fULL) + ((value >>  4) & 0x0f0f0f0f0f0f0f0fULL);
    value = (value & 0x00ff00ff00ff00ffULL) + ((value >>  8) & 0x00ff00ff00ff00ffULL);
    value = (value & 0x0000ffff0000ffffULL) + ((value >> 16) & 0x0000ffff0000ffffULL);
    value = (value & 0x00000000ffffffffULL) + ((value >> 32) & 0x00000000ffffffffULL);
    return (unsigned int)value;
}

static __attribute_const__ __always_inline
bool popparity8_const(uint8_t value)
{
    value ^= value >> 4;
    value &= 0xf;
    return (0x6996 >> value) & 1;
}

static __attribute_const__ __always_inline
bool popparity16_const(uint16_t value)
{
    value ^= value >> 8;
    value ^= value >> 4;
    value &= 0xf;
    return (0x6996 >> value) & 1;
}

static __attribute_const__ __always_inline
bool popparity32_const(uint32_t value)
{
    value ^= value >> 16;
    value ^= value >> 8;
    value ^= value >> 4;
    value &= 0xf;
    return (0x6996 >> value) & 1;
}

static __attribute_const__ __always_inline
bool popparity64_const(uint64_t value)
{
    value ^= value >> 32;
    value ^= value >> 16;
    value ^= value >> 8;
    value ^= value >> 4;
    value &= 0xf;
    return (0x6996 >> value) & 1;
}

static inline __attribute_const__
unsigned int popcount8_dynamic(uint8_t value)
{
    return popcount_table[value];
}

static inline __attribute_const__
unsigned int popcount16_dynamic(uint16_t value)
{
    uint8_t high = value >> 8;
    return popcount8_dynamic(high) +
           popcount8_dynamic((uint8_t)value);
}

static inline __attribute_const__
unsigned int popcount32_dynamic(uint32_t value)
{
    uint16_t high = value >> 16;
    return popcount16_dynamic(high) +
           popcount16_dynamic((uint16_t)value);
}

static inline __attribute_const__
unsigned int popcount64_dynamic(uint64_t value)
{
    uint32_t high = value >> 32;
    return popcount32_dynamic(high) +
           popcount32_dynamic((uint32_t)value);
}

static inline __attribute_const__
bool popparity8_dynamic(uint8_t value)
{
    return popparity_table[value];
}

static inline __attribute_const__
bool popparity16_dynamic(uint16_t value)
{
    uint8_t high = value >> 8;
    return (popparity8_dynamic((uint8_t)value) +
            popparity8_dynamic(high)) & 1;
}

static inline __attribute_const__
bool popparity32_dynamic(uint32_t value)
{
    uint16_t high = value >> 16;
    return (popparity16_dynamic((uint16_t)value) +
            popparity16_dynamic(high)) & 1;
}

static inline __attribute_const__
bool popparity64_dynamic(uint64_t value)
{
    uint32_t high = value >> 32;
    return (popparity32_dynamic((uint32_t)value) +
            popparity32_dynamic(high)) & 1;
}

#define popcount8(value) ({         \
    uint8_t __value = (value);      \
    __builtin_constant_p(__value)   \
    ? popcount8_const(__value)      \
    : popcount8_dynamic(__value);   \
})

#define popcount16(value) ({        \
    uint16_t __value = (value);     \
    __builtin_constant_p(__value)   \
    ? popcount16_const(__value)     \
    : popcount16_dynamic(__value);  \
})

#define popcount32(value) ({        \
    uint32_t __value = (value);     \
    __builtin_constant_p(__value)   \
    ? popcount32_const(__value)     \
    : popcount32_dynamic(__value);  \
})

#define popcount64(value) ({        \
    uint64_t __value = (value);     \
    __builtin_constant_p(__value)   \
    ? popcount64_const(__value)     \
    : popcount64_dynamic(__value);  \
})

#define popparity8(value) ({        \
    uint8_t __value = (value);      \
    __builtin_constant_p(__value)   \
    ? popparity8_const(__value)     \
    : popparity8_dynamic(__value);  \
})

#define popparity16(value) ({       \
    uint16_t __value = (value);     \
    __builtin_constant_p(__value)   \
    ? popparity16_const(__value)    \
    : popparity16_dynamic(__value); \
})

#define popparity32(value) ({       \
    uint32_t __value = (value);     \
    __builtin_constant_p(__value)   \
    ? popparity32_const(__value)    \
    : popparity32_dynamic(__value); \
})

#define popparity64(value) ({       \
    uint64_t __value = (value);     \
    __builtin_constant_p(__value)   \
    ? popparity64_const(__value)    \
    : popparity64_dynamic(__value); \
})

#endif  /* _POPCOUNT_H_ */
