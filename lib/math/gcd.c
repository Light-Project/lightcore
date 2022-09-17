/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2022 Sanpe <sanpeqf@gmail.com>
 */

#include <math.h>
#include <macro.h>
#include <bitops.h>
#include <export.h>

#define GENERIC_GCD_OPS(name, type, ops)    \
type name(type first, type second)          \
{                                           \
    type mixed = first | second;            \
                                            \
    second >>= ops(second);                 \
    if (second == 1)                        \
        return mixed & -mixed;              \
                                            \
    for (;;) {                              \
        first >>= ops(first);               \
        if (first == 1)                     \
            return mixed & -mixed;          \
                                            \
        if (first == second)                \
            return first << ops(mixed);     \
                                            \
        if (first < second)                 \
            swap(first, second);            \
                                            \
        first -= second;                    \
    }                                       \
}                                           \
EXPORT_SYMBOL(name)

GENERIC_GCD_OPS(gcdi, unsigned int, ffsuf);
GENERIC_GCD_OPS(gcdl, unsigned long, ffsuf);
GENERIC_GCD_OPS(gcdll, unsigned long long, ffsuf64);

GENERIC_GCD_OPS(gcd8, uint8_t, ffsuf);
GENERIC_GCD_OPS(gcd16, uint16_t, ffsuf);
GENERIC_GCD_OPS(gcd32, uint32_t, ffsuf);
GENERIC_GCD_OPS(gcd64, uint64_t, ffsuf64);
