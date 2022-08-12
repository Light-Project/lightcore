/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2022 Sanpe <sanpeqf@gmail.com>
 */

#include <math.h>
#include <export.h>

#define GENERIC_POW_OPS(name, type, ops)    \
type name(type base, unsigned int exp)      \
{                                           \
    type result = 1;                        \
                                            \
    while (exp) {                           \
        if (exp & 1)                        \
            result *= base;                 \
        exp ops;                            \
        base *= base;                       \
    }                                       \
                                            \
    return result;                          \
}                                           \
EXPORT_SYMBOL(name)

GENERIC_POW_OPS(powi, int, /= 2);
GENERIC_POW_OPS(powl, long, /= 2);
GENERIC_POW_OPS(powll, long long, /= 2);
GENERIC_POW_OPS(powu, unsigned int, >>= 1);
GENERIC_POW_OPS(powul, unsigned long, >>= 1);
GENERIC_POW_OPS(powull, unsigned long long, >>= 1);

GENERIC_POW_OPS(pows8, int8_t, /= 2);
GENERIC_POW_OPS(pows16, int16_t, /= 2);
GENERIC_POW_OPS(pows32, int32_t, /= 2);
GENERIC_POW_OPS(pows64, int64_t, /= 2);
GENERIC_POW_OPS(powu8, uint8_t, >>= 1);
GENERIC_POW_OPS(powu16, uint16_t, >>= 1);
GENERIC_POW_OPS(powu32, uint32_t, >>= 1);
GENERIC_POW_OPS(powu64, uint64_t, >>= 1);
