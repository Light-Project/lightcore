/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2022 Sanpe <sanpeqf@gmail.com>
 */

#include <math.h>
#include <export.h>

#define GENERIC_POW_OPS(name, type)     \
type name(type base, unsigned int exp)  \
{                                       \
    type result = 1;                    \
                                        \
    while (exp) {                       \
        if (exp & 1)                    \
            result *= base;             \
        exp >>= 1;                      \
        base *= base;                   \
    }                                   \
                                        \
    return result;                      \
}

GENERIC_POW_OPS(pows8, int8_t);
GENERIC_POW_OPS(pows16, int16_t);
GENERIC_POW_OPS(pows32, int32_t);
GENERIC_POW_OPS(pows64, int64_t);
GENERIC_POW_OPS(powu8, uint8_t);
GENERIC_POW_OPS(powu16, uint16_t);
GENERIC_POW_OPS(powu32, uint32_t);
GENERIC_POW_OPS(powu64, uint64_t);

#define POWI_ALIAS __stringify(__PASTE(pows, INT_WIDTH))
#define POWL_ALIAS __stringify(__PASTE(pows, LONG_WIDTH))
#define POWLL_ALIAS __stringify(__PASTE(pows, LLONG_WIDTH))
#define POWU_ALIAS __stringify(__PASTE(powu, INT_WIDTH))
#define POWUL_ALIAS __stringify(__PASTE(powu, LONG_WIDTH))
#define POWULL_ALIAS __stringify(__PASTE(powu, LLONG_WIDTH))

extern int powi(int base, unsigned int exp) __alias(POWI_ALIAS);
extern long powl(long base, unsigned int exp) __alias(POWL_ALIAS);
extern long long powll(long long base, unsigned int exp) __alias(POWLL_ALIAS);
extern unsigned int powu(unsigned int base, unsigned int exp) __alias(POWU_ALIAS);
extern unsigned long powul(unsigned long base, unsigned int exp) __alias(POWUL_ALIAS);
extern unsigned long long powull(unsigned long long base, unsigned int exp) __alias(POWULL_ALIAS);

EXPORT_SYMBOL(pows8);
EXPORT_SYMBOL(pows16);
EXPORT_SYMBOL(pows32);
EXPORT_SYMBOL(pows64);
EXPORT_SYMBOL(powu8);
EXPORT_SYMBOL(powu16);
EXPORT_SYMBOL(powu32);
EXPORT_SYMBOL(powu64);

EXPORT_SYMBOL(powi);
EXPORT_SYMBOL(powl);
EXPORT_SYMBOL(powll);
EXPORT_SYMBOL(powu);
EXPORT_SYMBOL(powul);
EXPORT_SYMBOL(powull);
