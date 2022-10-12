/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _HASH_H_
#define _HASH_H_

#include <types.h>
#include <bits.h>

#define GOLDEN_RATIO_32 0x61c88647UL
#define GOLDEN_RATIO_64 0x61c8864680b583ebULL

#ifdef CONFIG_ARCH_HAS_HASH
# include <asm/hash.h>
#endif

#ifndef ARCH_HAS_HASH32
# define __hash32 hash32_generic
#endif

#ifndef ARCH_HAS_HASH64
# define __hash64 hash64_generic
#endif

static __always_inline uint32_t hash32_generic(uint32_t value)
{
    return value * GOLDEN_RATIO_32;
}

static __always_inline uint64_t hash64_generic(uint64_t value)
{
    return value * GOLDEN_RATIO_64;
}

static __always_inline uint32_t hash32(uint32_t value, unsigned int bits)
{
    return __hash32(value) >> (BITS_PER_U32 - bits);
}

static __always_inline uint32_t hash64(uint64_t value, unsigned int bits)
{
    return __hash64(value) >> (BITS_PER_U64 - bits);
}

#endif  /* _HASH_H_ */