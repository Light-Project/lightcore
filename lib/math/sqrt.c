/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2022 Sanpe <sanpeqf@gmail.com>
 */

#include <math.h>
#include <bitops.h>
#include <export.h>

#define GENERIC_SQRT_OPS(name, type, ops)   \
type name(type value)                       \
{                                           \
    type result = 0;                        \
    type tmp, loop;                         \
                                            \
    if (value < 2)                          \
        return value;                       \
                                            \
    loop = BIT_ULL(ops(value) & ~1ULL);     \
    while (loop != 0) {                     \
        tmp = result + loop;                \
        result >>= 1;                       \
                                            \
        if (value >= tmp) {                 \
            value -= tmp;                   \
            result += loop;                 \
        }                                   \
                                            \
        loop >>= 2;                         \
    }                                       \
                                            \
    return result;                          \
}                                           \
EXPORT_SYMBOL(name)

GENERIC_SQRT_OPS(sqrti, unsigned int, flsuf);
GENERIC_SQRT_OPS(sqrtl, unsigned long, flsuf);
GENERIC_SQRT_OPS(sqrtll, unsigned long long, flsuf64);

GENERIC_SQRT_OPS(sqrt8, uint8_t, flsuf);
GENERIC_SQRT_OPS(sqrt16, uint16_t, flsuf);
GENERIC_SQRT_OPS(sqrt32, uint32_t, flsuf);
GENERIC_SQRT_OPS(sqrt64, uint64_t, flsuf64);
