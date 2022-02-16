/* SPDX-License-Identifier: GPL-2.0-or-later */
#include <hash.h>
#include <bitops.h>
#include <asm/unaligned.h>

#ifdef CONFIG_ARCH_32BIT

#define HASH_MIX(x, y, a) (     \
    x ^= (a),                   \
    y ^= x, x = rol32(x, 7),    \
    x += y, y = rol32(y, 20),   \
    y *= 9                      \
)

static inline unsigned int fold_hash(unsigned long x, unsigned long y)
{
    y ^= x * GOLDEN_RATIO_32;
    y *= GOLDEN_RATIO_32;
    return y >> 32;
}

#else

#define HASH_MIX(x, y, a) (     \
    x ^= (a),                   \
    y ^= x, x = rol64(x, 12),   \
    x += y, y = rol64(y, 45),   \
    y *= 9                      \
)

static inline unsigned int fold_hash(unsigned long x, unsigned long y)
{
    y ^= x * GOLDEN_RATIO_64;
    y *= GOLDEN_RATIO_64;
    return y >> 32;
}

#endif

uint64_t hashlen_string(const char *str, unsigned long salt)
{
    uint32_t hash = 5381;

    while (*str)
        hash = (hash << 5) + hash ^ *str++;

    return hash;
}
