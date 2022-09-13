/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _POPCOUNT_H_
#define _POPCOUNT_H_

#include <types.h>

static inline unsigned int popcount8(uint8_t value)
{
    value = (value & 0x55) + ((value >> 1) & 0x55);
    value = (value & 0x33) + ((value >> 2) & 0x33);
    value = (value & 0x0f) + ((value >> 4) & 0x0f);
    return (unsigned int)value;
}

static inline unsigned int popcount16(uint16_t value)
{
    value = (value & 0x5555) + ((value >> 1) & 0x5555);
    value = (value & 0x3333) + ((value >> 2) & 0x3333);
    value = (value & 0x0f0f) + ((value >> 4) & 0x0f0f);
    value = (value & 0x00ff) + ((value >> 8) & 0x00ff);
    return (unsigned int)value;
}

static inline unsigned int popcount32(uint32_t value)
{
    value = (value & 0x55555555UL) + ((value >>  1) & 0x55555555);
    value = (value & 0x33333333UL) + ((value >>  2) & 0x33333333);
    value = (value & 0x0f0f0f0fUL) + ((value >>  4) & 0x0f0f0f0f);
    value = (value & 0x00ff00ffUL) + ((value >>  8) & 0x00ff00ff);
    value = (value & 0x0000ffffUL) + ((value >> 16) & 0x0000ffff);
    return (unsigned int)value;
}

static inline unsigned int popcount64(uint64_t value)
{
    value = (value & 0x5555555555555555ULL) + ((value >>  1) & 0x5555555555555555ULL);
    value = (value & 0x3333333333333333ULL) + ((value >>  2) & 0x3333333333333333ULL);
    value = (value & 0x0f0f0f0f0f0f0f0fULL) + ((value >>  4) & 0x0f0f0f0f0f0f0f0fULL);
    value = (value & 0x00ff00ff00ff00ffULL) + ((value >>  8) & 0x00ff00ff00ff00ffULL);
    value = (value & 0x0000ffff0000ffffULL) + ((value >> 16) & 0x0000ffff0000ffffULL);
    value = (value & 0x00000000ffffffffULL) + ((value >> 32) & 0x00000000ffffffffULL);
    return (unsigned int)value;
}

static inline bool popparity8(uint8_t value)
{
    return popcount8(value) & 0x01;
}

static inline bool popparity16(uint16_t value)
{
    return popcount16(value) & 0x01;
}

static inline bool popparity32(uint32_t value)
{
    return popcount32(value) & 0x01;
}

static inline bool popparity64(uint64_t value)
{
    return popcount64(value) & 0x01;
}

#endif  /* _POPCOUNT_H_ */
