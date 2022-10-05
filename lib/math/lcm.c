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
}                                           \
EXPORT_SYMBOL(name)

GENERIC_LCM_OPS(lcmi, unsigned int, gcdi);
GENERIC_LCM_OPS(lcml, unsigned long, gcdl);
GENERIC_LCM_OPS(lcmll, unsigned long long, gcdll);

GENERIC_LCM_OPS(lcm8, uint8_t, gcd8);
GENERIC_LCM_OPS(lcm16, uint16_t, gcd16);
GENERIC_LCM_OPS(lcm32, uint32_t, gcd32);
GENERIC_LCM_OPS(lcm64, uint64_t, gcd64);
