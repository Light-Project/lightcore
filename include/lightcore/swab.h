/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _LIGHTCORE_SWAB_H_
#define _LIGHTCORE_SWAB_H_

#include <lightcore/types.h>
#include <lightcore/asm/bitsperlong.h>

/*
 * casts are necessary for constants, because we never know how for sure
 * how U/UL/ULL map to __u16, __u32, __u64. At least not in a portable way.
 */
#define constant_swab16(x) ((__u16)(                        \
    (((__u16)(x) & (__u16)0x00ffU) << 8) |                  \
    (((__u16)(x) & (__u16)0xff00U) >> 8)))

#define constant_swab32(x) ((__u32)(                        \
    (((__u32)(x) & (__u32)0x000000ffUL) << 24) |            \
    (((__u32)(x) & (__u32)0x0000ff00UL) <<  8) |            \
    (((__u32)(x) & (__u32)0x00ff0000UL) >>  8) |            \
    (((__u32)(x) & (__u32)0xff000000UL) >> 24)))

#define constant_swab64(x) ((__u64)(                        \
    (((__u64)(x) & (__u64)0x00000000000000ffULL) << 56) |   \
    (((__u64)(x) & (__u64)0x000000000000ff00ULL) << 40) |   \
    (((__u64)(x) & (__u64)0x0000000000ff0000ULL) << 24) |   \
    (((__u64)(x) & (__u64)0x00000000ff000000ULL) <<  8) |   \
    (((__u64)(x) & (__u64)0x000000ff00000000ULL) >>  8) |   \
    (((__u64)(x) & (__u64)0x0000ff0000000000ULL) >> 24) |   \
    (((__u64)(x) & (__u64)0x00ff000000000000ULL) >> 40) |   \
    (((__u64)(x) & (__u64)0xff00000000000000ULL) >> 56)))

#define constant_swahw32(x) ((__u32)(                       \
    (((__u32)(x) & (__u32)0x0000ffffUL) << 16) |            \
    (((__u32)(x) & (__u32)0xffff0000UL) >> 16)))

#define constant_swahb32(x) ((__u32)(                       \
    (((__u32)(x) & (__u32)0x00ff00ffUL) << 8) |             \
    (((__u32)(x) & (__u32)0xff00ff00UL) >> 8)))

/*
 * Implement the following as inlines, but define the interface using
 * macros to allow constant folding when possible:
 * _swab16, _swab32, _swab64, ___swahw32, _swahb32
 */

static inline __attribute_const__ __u16 fswab16(__u16 val)
{
#if defined (arch_swab16)
    return arch_swab16(val);
#else
    return constant_swab16(val);
#endif
}

static inline __attribute_const__ __u32 fswab32(__u32 val)
{
#if defined(arch_swab32)
    return arch_swab32(val);
#else
    return constant_swab32(val);
#endif
}

static inline __attribute_const__ __u64 fswab64(__u64 val)
{
#if defined (arch_swab64)
    return arch_swab64(val);
#elif defined(swab_64_THRU_32__)
    __u32 h = val >> 32;
    __u32 l = val & ((1ULL << 32) - 1);
    return (((__u64)fswab32(l)) << 32) | ((__u64)(fswab32(h)));
#else
    return constant_swab64(val);
#endif
}

static inline __attribute_const__ __u32 fswahw32(__u32 val)
{
#ifdef arch_swahw32
    return arch_swahw32(val);
#else
    return constant_swahw32(val);
#endif
}

static inline __attribute_const__ __u32 fswahb32(__u32 val)
{
#ifdef arch_swahb32
    return arch_swahb32(val);
#else
    return constant_swahb32(val);
#endif
}

/**
 * swab16 - return a byteswapped 16-bit value
 * @x: value to byteswap
 */
#ifdef __HAVE_BUILTIN_BSWAP16__
#define swab16(x) (__u16)__builtin_bswap16((__u16)(x))
#else
#define swab16(x)                               \
    (__builtin_constant_p((__u16)(x)) ?         \
    constant_swab16(x) :                        \
    fswab16(x))
#endif

/**
 * swab32 - return a byteswapped 32-bit value
 * @x: value to byteswap
 */
#ifdef __HAVE_BUILTIN_BSWAP32__
#define swab32(x) (__u32)__builtin_bswap32((__u32)(x))
#else
#define swab32(x)                               \
    (__builtin_constant_p((__u32)(x)) ?         \
    constant_swab32(x) :                        \
    fswab32(x))
#endif

/**
 * swab64 - return a byteswapped 64-bit value
 * @x: value to byteswap
 */
#ifdef __HAVE_BUILTIN_BSWAP64__
#define swab64(x) (__u64)__builtin_bswap64((__u64)(x))
#else
#define swab64(x)                               \
    (__builtin_constant_p((__u64)(x)) ?         \
    constant_swab64(x) :                        \
    fswab64(x))
#endif

static __always_inline unsigned long swab(const unsigned long y)
{
#if __BITS_PER_LONG == 64
    return swab64(y);
#else /* __BITS_PER_LONG == 32 */
    return swab32(y);
#endif
}

/**
 * __swahw32 - return a word-swapped 32-bit value
 * @x: value to wordswap
 *
 * __swahw32(0x12340000) is 0x00001234
 */
#define swahw32(x)                          \
    (__builtin_constant_p((__u32)(x)) ?     \
    constant_swahw32(x) :                   \
    fswahw32(x))

/**
 * swahb32 - return a high and low byte-swapped 32-bit value
 * @x: value to byteswap
 *
 * swahb32(0x12345678) is 0x34127856
 */
#define swahb32(x)                          \
    (__builtin_constant_p((__u32)(x)) ?     \
    constant_swahb32(x) :                   \
    fswahb32(x))

/**
 * swab16p - return a byteswapped 16-bit value from a pointer
 * @p: pointer to a naturally-aligned 16-bit value
 */
static __always_inline __u16 swab16p(const __u16 *p)
{
#ifdef arch_swab16p
    return arch_swab16p(p);
#else
    return swab16(*p);
#endif
}

/**
 * swab32p - return a byteswapped 32-bit value from a pointer
 * @p: pointer to a naturally-aligned 32-bit value
 */
static __always_inline __u32 swab32p(const __u32 *p)
{
#ifdef arch_swab32p
    return arch_swab32p(p);
#else
    return swab32(*p);
#endif
}

/**
 * swab64p - return a byteswapped 64-bit value from a pointer
 * @p: pointer to a naturally-aligned 64-bit value
 */
static __always_inline __u64 swab64p(const __u64 *p)
{
#ifdef arch_swab64p
    return arch_swab64p(p);
#else
    return swab64(*p);
#endif
}

/**
 * swahw32p - return a wordswapped 32-bit value from a pointer
 * @p: pointer to a naturally-aligned 32-bit value
 *
 * See __swahw32() for details of wordswapping.
 */
static inline __u32 swahw32p(const __u32 *p)
{
#ifdef arch_swahw32p
    return arch_swahw32p(p);
#else
    return swahw32(*p);
#endif
}

/**
 * swahb32p - return a high and low byteswapped 32-bit value from a pointer
 * @p: pointer to a naturally-aligned 32-bit value
 *
 * See swahb32() for details of high/low byteswapping.
 */
static inline __u32 swahb32p(const __u32 *p)
{
#ifdef arch_swahb32p
    return arch_swahb32p(p);
#else
    return swahb32(*p);
#endif
}

/**
 * swab16s - byteswap a 16-bit value in-place
 * @p: pointer to a naturally-aligned 16-bit value
 */
static inline void swab16s(__u16 *p)
{
#ifdef arch_swab16s
    arch_swab16s(p);
#else
    *p = swab16p(p);
#endif
}
/**
 * swab32s - byteswap a 32-bit value in-place
 * @p: pointer to a naturally-aligned 32-bit value
 */
static __always_inline void swab32s(__u32 *p)
{
#ifdef arch_swab32s
    arch_swab32s(p);
#else
    *p = swab32p(p);
#endif
}

/**
 * swab64s - byteswap a 64-bit value in-place
 * @p: pointer to a naturally-aligned 64-bit value
 */
static __always_inline void swab64s(__u64 *p)
{
#ifdef arch_swab64s
    arch_swab64s(p);
#else
    *p = swab64p(p);
#endif
}

/**
 * __swahw32s - wordswap a 32-bit value in-place
 * @p: pointer to a naturally-aligned 32-bit value
 *
 * See __swahw32() for details of wordswapping
 */
static inline void swahw32s(__u32 *p)
{
#ifdef arch_swahw32s
    arch_swahw32s(p);
#else
    *p = swahw32p(p);
#endif
}

/**
 * swahb32s - high and low byteswap a 32-bit value in-place
 * @p: pointer to a naturally-aligned 32-bit value
 *
 * See swahb32() for details of high and low byte swapping
 */
static inline void swahb32s(__u32 *p)
{
#ifdef arch_swahb32s
    arch_swahb32s(p);
#else
    *p = swahb32p(p);
#endif
}

#endif /* _LIGHTCORE_SWAB_H_ */
