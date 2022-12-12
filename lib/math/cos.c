/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2022 Sanpe <sanpeqf@gmail.com>
 */

#include <math.h>
#include <export.h>

#define GENERIC_COS_OPS(name, type, ops)    \
type cos##name(int angle)                   \
{                                           \
    return ops(angle + 90);                 \
}                                           \
EXPORT_SYMBOL(cos##name)

GENERIC_COS_OPS(16, int16_t, sin16);
GENERIC_COS_OPS(32, int32_t, sin32);
