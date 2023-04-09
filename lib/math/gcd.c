/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2022 John Sanpe <sanpeqf@gmail.com>
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
}

GENERIC_GCD_OPS(gcd8, uint8_t, ffsuf);
GENERIC_GCD_OPS(gcd16, uint16_t, ffsuf);
GENERIC_GCD_OPS(gcd32, uint32_t, ffsuf);
GENERIC_GCD_OPS(gcd64, uint64_t, ffsuf64);

#define GCDI_ALIAS __stringify(__PASTE(gcd, INT_WIDTH))
#define GCDL_ALIAS __stringify(__PASTE(gcd, LONG_WIDTH))
#define GCDLL_ALIAS __stringify(__PASTE(gcd, LLONG_WIDTH))

extern unsigned int gcdi(unsigned int first, unsigned int second) __alias(GCDI_ALIAS);
extern unsigned long gcdl(unsigned long first, unsigned long second) __alias(GCDL_ALIAS);
extern unsigned long long gcdll(unsigned long long first, unsigned long long second) __alias(GCDLL_ALIAS);

EXPORT_SYMBOL(gcd8);
EXPORT_SYMBOL(gcd16);
EXPORT_SYMBOL(gcd32);
EXPORT_SYMBOL(gcd64);

EXPORT_SYMBOL(gcdi);
EXPORT_SYMBOL(gcdl);
EXPORT_SYMBOL(gcdll);
