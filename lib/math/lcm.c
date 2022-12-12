/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2022 Sanpe <sanpeqf@gmail.com>
 */

#include <math.h>
#include <macro.h>
#include <export.h>

#define GENERIC_LCM_OPS(name, type, ops)    \
type name(type first, type second)          \
{                                           \
    if (!first || !second)                  \
        return 0;                           \
                                            \
    return (first / ops(first,              \
            second)) * second;              \
}

GENERIC_LCM_OPS(lcm8, uint8_t, gcd8);
GENERIC_LCM_OPS(lcm16, uint16_t, gcd16);
GENERIC_LCM_OPS(lcm32, uint32_t, gcd32);
GENERIC_LCM_OPS(lcm64, uint64_t, gcd64);

#define LCMI_ALIAS __stringify(__PASTE(lcm, INT_WIDTH))
#define LCML_ALIAS __stringify(__PASTE(lcm, LONG_WIDTH))
#define LCMLL_ALIAS __stringify(__PASTE(lcm, LLONG_WIDTH))

extern unsigned int lcmi(unsigned int first, unsigned int second) __alias(LCMI_ALIAS);
extern unsigned long lcml(unsigned long first, unsigned long second) __alias(LCML_ALIAS);
extern unsigned long long lcmll(unsigned long long first, unsigned long long second) __alias(LCMLL_ALIAS);

EXPORT_SYMBOL(lcm8);
EXPORT_SYMBOL(lcm16);
EXPORT_SYMBOL(lcm32);
EXPORT_SYMBOL(lcm64);

EXPORT_SYMBOL(lcmi);
EXPORT_SYMBOL(lcml);
EXPORT_SYMBOL(lcmll);
