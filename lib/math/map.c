/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2022 John Sanpe <sanpeqf@gmail.com>
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
}

GENERIC_MAP_OPS(maps8, int8_t);
GENERIC_MAP_OPS(maps16, int16_t);
GENERIC_MAP_OPS(maps32, int32_t);
GENERIC_MAP_OPS(maps64, int64_t);
GENERIC_MAP_OPS(mapu8, uint8_t);
GENERIC_MAP_OPS(mapu16, uint16_t);
GENERIC_MAP_OPS(mapu32, uint32_t);
GENERIC_MAP_OPS(mapu64, uint64_t);

#define MAPI_ALIAS __stringify(__PASTE(maps, INT_WIDTH))
#define MAPL_ALIAS __stringify(__PASTE(maps, LONG_WIDTH))
#define MAPLL_ALIAS __stringify(__PASTE(maps, LLONG_WIDTH))
#define MAPU_ALIAS __stringify(__PASTE(mapu, INT_WIDTH))
#define MAPUL_ALIAS __stringify(__PASTE(mapu, LONG_WIDTH))
#define MAPULL_ALIAS __stringify(__PASTE(mapu, LLONG_WIDTH))

extern int mapi(int mini, int maxi, int mino, int maxo, int value) __alias(MAPI_ALIAS);
extern long mapl(long mini, long maxi, long mino, long maxo, long value) __alias(MAPL_ALIAS);
extern long long mapll(long long mini, long long maxi, long long mino, long long maxo, long long value) __alias(MAPLL_ALIAS);
extern unsigned int mapu(unsigned int mini, unsigned int maxi, unsigned int mino, unsigned int maxo, unsigned int value) __alias(MAPU_ALIAS);
extern unsigned long mapul(unsigned long mini, unsigned long maxi, unsigned long mino, unsigned long maxo, unsigned long value) __alias(MAPUL_ALIAS);
extern unsigned long long mapull(unsigned long long mini, unsigned long long maxi, unsigned long long mino, unsigned long long maxo, unsigned long long value) __alias(MAPULL_ALIAS);

EXPORT_SYMBOL(maps8);
EXPORT_SYMBOL(maps16);
EXPORT_SYMBOL(maps32);
EXPORT_SYMBOL(maps64);
EXPORT_SYMBOL(mapu8);
EXPORT_SYMBOL(mapu16);
EXPORT_SYMBOL(mapu32);
EXPORT_SYMBOL(mapu64);

EXPORT_SYMBOL(mapi);
EXPORT_SYMBOL(mapl);
EXPORT_SYMBOL(mapll);
EXPORT_SYMBOL(mapu);
EXPORT_SYMBOL(mapul);
EXPORT_SYMBOL(mapull);
