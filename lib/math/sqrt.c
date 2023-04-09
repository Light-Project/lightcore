/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2022 John Sanpe <sanpeqf@gmail.com>
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
}

GENERIC_SQRT_OPS(sqrt8, uint8_t, flsuf);
GENERIC_SQRT_OPS(sqrt16, uint16_t, flsuf);
GENERIC_SQRT_OPS(sqrt32, uint32_t, flsuf);
GENERIC_SQRT_OPS(sqrt64, uint64_t, flsuf64);

#define SQRTI_ALIAS __stringify(__PASTE(sqrt, INT_WIDTH))
#define SQRTL_ALIAS __stringify(__PASTE(sqrt, LONG_WIDTH))
#define SQRTLL_ALIAS __stringify(__PASTE(sqrt, LLONG_WIDTH))

extern unsigned int sqrti(unsigned int value) __alias(SQRTI_ALIAS);
extern unsigned long sqrtl(unsigned long value) __alias(SQRTL_ALIAS);
extern unsigned long long sqrtll(unsigned long long value) __alias(SQRTLL_ALIAS);

EXPORT_SYMBOL(sqrt8);
EXPORT_SYMBOL(sqrt16);
EXPORT_SYMBOL(sqrt32);
EXPORT_SYMBOL(sqrt64);

EXPORT_SYMBOL(sqrti);
EXPORT_SYMBOL(sqrtl);
EXPORT_SYMBOL(sqrtll);
