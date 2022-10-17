/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2022 Sanpe <sanpeqf@gmail.com>
 */

#include <math.h>
#include <macro.h>
#include <export.h>

#define GENERIC_MAP_OPS(name, type)             \
type name(type mini, type maxi, type mino,      \
          type maxo, type value)                \
{                                               \
    type deltai, deltao;                        \
                                                \
    if (value <= mini)                          \
        return mino;                            \
    if (value >= maxi)                          \
        return maxo;                            \
                                                \
    deltai = maxi - mini;                       \
    deltao = maxo - mino;                       \
                                                \
    return ((value - mini) * deltao) /          \
            deltai + mino;                      \
}                                               \
EXPORT_SYMBOL(name)

GENERIC_MAP_OPS(mapi, int);
GENERIC_MAP_OPS(mapl, long);
GENERIC_MAP_OPS(mapll, long long);
GENERIC_MAP_OPS(mapu, unsigned int);
GENERIC_MAP_OPS(mapul, unsigned long);
GENERIC_MAP_OPS(mapull, unsigned long long);

GENERIC_MAP_OPS(maps8, int8_t);
GENERIC_MAP_OPS(maps16, int16_t);
GENERIC_MAP_OPS(maps32, int32_t);
GENERIC_MAP_OPS(maps64, int64_t);
GENERIC_MAP_OPS(mapu8, uint8_t);
GENERIC_MAP_OPS(mapu16, uint16_t);
GENERIC_MAP_OPS(mapu32, uint32_t);
GENERIC_MAP_OPS(mapu64, uint64_t);
