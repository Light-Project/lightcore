/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _ASM_GENERIC_BITOPS_ENDIAN_H_
#define _ASM_GENERIC_BITOPS_ENDIAN_H_

#include <types.h>

#if defined(CONFIG_ARCH_LITTLE_ENDIAN)
#define BITOP_LE_SWIZZLE 0
#define BITOP_BE_SWIZZLE ((BITS_PER_LONG-1) & ~0x7)
#elif defined(CONFIG_ARCH_BIG_ENDIAN)
#define BITOP_LE_SWIZZLE ((BITS_PER_LONG-1) & ~0x7)
#define BITOP_BE_SWIZZLE 0
#endif

static inline void bit_clr_le(void *addr, long bit)
{
    bit_clr(addr, bit ^ BITOP_LE_SWIZZLE);
}

static inline void bit_set_le(void *addr, long bit)
{
    bit_set(addr, bit ^ BITOP_LE_SWIZZLE);
}

static inline void bit_flip_le(void *addr, long bit)
{
    bit_flip(addr, bit ^ BITOP_LE_SWIZZLE);
}

static inline bool bit_test_le(void *addr, long bit)
{
    return bit_test(addr, bit ^ BITOP_LE_SWIZZLE);
}

static inline bool bit_test_clr_le(void *addr, long bit)
{
    return bit_test_clr(addr, bit ^ BITOP_LE_SWIZZLE);
}

static inline bool bit_test_set_le(void *addr, long bit)
{
    return bit_test_set(addr, bit ^ BITOP_LE_SWIZZLE);
}

static inline bool bit_test_flip_le(void *addr, long bit)
{
    return bit_test_flip(addr, bit ^ BITOP_LE_SWIZZLE);
}

static inline void bit_atomic_clr_le(void *addr, long bit)
{
    bit_atomic_clr(addr, bit ^ BITOP_LE_SWIZZLE);
}

static inline void bit_atomic_set_le(void *addr, long bit)
{
    bit_atomic_set(addr, bit ^ BITOP_LE_SWIZZLE);
}

static inline void bit_atomic_flip_le(void *addr, long bit)
{
    bit_atomic_flip(addr, bit ^ BITOP_LE_SWIZZLE);
}

static inline bool bit_atomic_test_le(void *addr, long bit)
{
    return bit_atomic_test(addr, bit ^ BITOP_LE_SWIZZLE);
}

static inline bool bit_atomic_test_clr_le(void *addr, long bit)
{
    return bit_atomic_test_clr(addr, bit ^ BITOP_LE_SWIZZLE);
}

static inline bool bit_atomic_test_set_le(void *addr, long bit)
{
    return bit_atomic_test_set(addr, bit ^ BITOP_LE_SWIZZLE);
}

static inline bool bit_atomic_test_flip_le(void *addr, long bit)
{
    return bit_atomic_test_flip(addr, bit ^ BITOP_LE_SWIZZLE);
}

static inline void bit_clr_be(void *addr, long bit)
{
    bit_clr(addr, bit ^ BITOP_BE_SWIZZLE);
}

static inline void bit_set_be(void *addr, long bit)
{
    bit_set(addr, bit ^ BITOP_BE_SWIZZLE);
}

static inline void bit_flip_be(void *addr, long bit)
{
    bit_flip(addr, bit ^ BITOP_BE_SWIZZLE);
}

static inline bool bit_test_be(void *addr, long bit)
{
    return bit_test(addr, bit ^ BITOP_BE_SWIZZLE);
}

static inline bool bit_test_clr_be(void *addr, long bit)
{
    return bit_test_clr(addr, bit ^ BITOP_BE_SWIZZLE);
}

static inline bool bit_test_set_be(void *addr, long bit)
{
    return bit_test_set(addr, bit ^ BITOP_BE_SWIZZLE);
}

static inline bool bit_test_flip_be(void *addr, long bit)
{
    return bit_test_flip(addr, bit ^ BITOP_BE_SWIZZLE);
}

static inline void bit_atomic_clr_be(void *addr, long bit)
{
    bit_atomic_clr(addr, bit ^ BITOP_BE_SWIZZLE);
}

static inline void bit_atomic_set_be(void *addr, long bit)
{
    bit_atomic_set(addr, bit ^ BITOP_BE_SWIZZLE);
}

static inline void bit_atomic_flip_be(void *addr, long bit)
{
    bit_atomic_flip(addr, bit ^ BITOP_BE_SWIZZLE);
}

static inline bool bit_atomic_test_be(void *addr, long bit)
{
    return bit_atomic_test(addr, bit ^ BITOP_BE_SWIZZLE);
}

static inline bool bit_atomic_test_clr_be(void *addr, long bit)
{
    return bit_atomic_test_clr(addr, bit ^ BITOP_BE_SWIZZLE);
}

static inline bool bit_atomic_test_set_be(void *addr, long bit)
{
    return bit_atomic_test_set(addr, bit ^ BITOP_BE_SWIZZLE);
}

static inline bool bit_atomic_test_flip_be(void *addr, long bit)
{
    return bit_atomic_test_flip(addr, bit ^ BITOP_BE_SWIZZLE);
}

#endif  /* _ASM_GENERIC_BITOPS_ENDIAN_H_ */
