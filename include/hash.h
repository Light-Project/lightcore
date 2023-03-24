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
# define hashv32 hashv32_generic
#endif

#ifndef ARCH_HAS_HASH64
# define hashv64 hashv64_generic
#endif

/**
 * hashv{32/64}_generic() - General func for calc hash value.
 * @value: value to hash.
 */
static __always_inline
uint32_t hashv32_generic(uint32_t value)
{
    /* Gold ratio method calc hash value. */
    return value * GOLDEN_RATIO_32;
}

static __always_inline
uint64_t hashv64_generic(uint64_t value)
{
    return value * GOLDEN_RATIO_64;
}

/**
 * hashv{32/64}() - Fast hashing routine for ints.
 * @value: value to hash.
 * @bits: bit number of result.
 */
static __always_inline
uint32_t hash32(uint32_t value, unsigned int bits)
{
    /* High bits are more random, so use them. */
    return hashv32(value) >> (BITS_PER_U32 - bits);
}

static __always_inline
uint64_t hash64(uint64_t value, unsigned int bits)
{
    return hashv64(value) >> (BITS_PER_U64 - bits);
}

#if BITS_PER_LONG == 32
# define GOLDEN_RATIO_LONG GOLDEN_RATIO_32
# define hashvl(value) hashv32(value)
# define hashl(value, bits) hash32(value, bits)
#else /* BITS_PER_LONG == 64 */
# define GOLDEN_RATIO_LONG GOLDEN_RATIO_64
# define hashvl(value) hashv64(value)
# define hashl(value, bits) hash64(value, bits)
#endif

#endif /* _HASH_H_ */
