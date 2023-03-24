/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _MATH_H_
#define _MATH_H_

#include <types.h>
#include <macro.h>

/*
 * This looks more complex than it should be. But we need to
 * get the type for the ~ right in round_down (it needs to be
 * as wide as the result!), and we want to evaluate the macro
 * arguments just once each.
 */
#define round_mask(x, y) ( \
    (typeof(x))((y) - 1) \
)

/**
 * round_up - round up to next specified power of 2.
 * @x: the value to round.
 * @y: multiple to round up to (must be a power of 2).
 */
#define round_up(x, y) ( \
    (((x) - 1) | round_mask(x, y)) + 1 \
)

/**
 * round_down - round down to next specified power of 2.
 * @x: the value to round.
 * @y: multiple to round down to (must be a power of 2).
 */
#define round_down(x, y) ( \
    (x) & ~round_mask(x, y) \
)

/**
 * DIV_ROUND_UP - round up division.
 * @n: divisor number.
 * @d: dividend number.
 */
#define DIV_ROUND_UP(n, d) ( \
    ((n) + (d) - 1) / (d) \
)

/*
 * Divide positive or negative dividend by positive or negative divisor
 * and round to closest integer. Result is undefined for negative
 * divisors if the dividend variable type is unsigned and for negative
 * dividends if the divisor variable type is unsigned.
 */
#define DIV_ROUND_CLOSEST(x, divisor) ({    \
    typeof(x) __x = x;                      \
    typeof(divisor) __d = divisor;          \
    (((typeof(x)) -1) > 0 ||                \
     ((typeof(divisor)) -1) > 0 ||          \
     (((__x) > 0) == ((__d) > 0))) ?        \
        (((__x) + ((__d) / 2)) / (__d)) :   \
        (((__x) - ((__d) / 2)) / (__d));    \
})

#define abs_choose_expr(x, type, other) __builtin_choose_expr(  \
    __builtin_types_compatible_p(typeof(x), signed type) ||     \
    __builtin_types_compatible_p(typeof(x), unsigned type),     \
    ({ signed type __x = (x); __x < 0 ? -__x : __x; }), other   \
)

#define abs(x) abs_choose_expr(x, long long,                \
    abs_choose_expr(x, long, abs_choose_expr(x, int,        \
    abs_choose_expr(x, short, abs_choose_expr(x, char,      \
    __builtin_choose_expr( __builtin_types_compatible_p(    \
    typeof(x), char), (char)({ signed char __x = (x);       \
    __x < 0 ? -__x : __x; }), ((void)0))))))                \
)

#define mult_frac(value, numer, denom) ({               \
    typeof(x) __quot = (value) / (denom);               \
    typeof(x) __rem = (value) % (denom);                \
    (__quot * (numer)) + ((__rem * (numer)) / (denom)); \
})

#ifdef TYPE_HAS_INT128

static inline uint64_t mul_u64_u32_shr(uint64_t value, uint32_t mul, unsigned int shift)
{
    return (uint64_t)(((uint128_t)value * mul) >> shift);
}

#else /* !TYPE_HAS_INT128 */

static inline uint64_t mul_u64_u32_shr(uint64_t value, uint32_t mul, unsigned int shift)
{
    uint32_t high, low;
    uint64_t retval;

    low = lower_32_bits(value);
    high = upper_32_bits(value);

    retval = (low * mul) >> shift;
    if (high)
        retval += (high * mul) << (32 - shift);

    return retval;
}

#endif /* TYPE_HAS_INT128 */

static inline uint64_t mul_u32_u32(uint32_t a, uint32_t b)
{
    return (uint64_t)a * b;
}

#define BEZIER_VALUE_SHIFT 10
#define BEZIER_VALUE_MAX (1 << BEZIER_VALUE_SHIFT)

extern int16_t sin16(int angle);
extern int32_t sin32(int angle);
extern int16_t cos16(int angle);
extern int32_t cos32(int angle);
extern unsigned int atan2i(int x, int y);
extern uint32_t bezier3(uint32_t time, uint32_t p0, uint32_t p1, uint32_t p2, uint32_t p3);

extern bool prime_check(unsigned long num);
extern unsigned long prime_next(unsigned long num);

extern unsigned int gcdi(unsigned int first, unsigned int second);
extern unsigned long gcdl(unsigned long first, unsigned long second);
extern unsigned long long gcdll(unsigned long long first, unsigned long long second);
extern uint8_t gcd8(uint8_t first, uint8_t second);
extern uint16_t gcd16(uint16_t first, uint16_t second);
extern uint32_t gcd32(uint32_t first, uint32_t second);
extern uint64_t gcd64(uint64_t first, uint64_t second);

extern unsigned int lcmi(unsigned int first, unsigned int second);
extern unsigned long lcml(unsigned long first, unsigned long second);
extern unsigned long long lcmll(unsigned long long first, unsigned long long second);
extern uint8_t lcm8(uint8_t first, uint8_t second);
extern uint16_t lcm16(uint16_t first, uint16_t second);
extern uint32_t lcm32(uint32_t first, uint32_t second);
extern uint64_t lcm64(uint64_t first, uint64_t second);

extern int mapi(int mini, int maxi, int mino, int maxo, int value);
extern long mapl(long mini, long maxi, long mino, long maxo, long value);
extern long long mapll(long long mini, long long maxi, long long mino, long long maxo, long long value);
extern unsigned int mapu(unsigned int mini, unsigned int maxi, unsigned int mino, unsigned int maxo, unsigned int value);
extern unsigned long mapul(unsigned long mini, unsigned long maxi, unsigned long mino, unsigned long maxo, unsigned long value);
extern unsigned long long mapull(unsigned long long mini, unsigned long long maxi, unsigned long long mino, unsigned long long maxo, unsigned long long value);
extern int8_t maps8(int8_t mini, int8_t maxi, int8_t mino, int8_t maxo, int8_t value);
extern int16_t maps16(int16_t mini, int16_t maxi, int16_t mino, int16_t maxo, int16_t value);
extern int32_t maps32(int32_t mini, int32_t maxi, int32_t mino, int32_t maxo, int32_t value);
extern int64_t maps64(int64_t mini, int64_t maxi, int64_t mino, int64_t maxo, int64_t value);
extern uint8_t mapu8(uint8_t mini, uint8_t maxi, uint8_t mino, uint8_t maxo, uint8_t value);
extern uint16_t mapu16(uint16_t mini, uint16_t maxi, uint16_t mino, uint16_t maxo, uint16_t value);
extern uint32_t mapu32(uint32_t mini, uint32_t maxi, uint32_t mino, uint32_t maxo, uint32_t value);
extern uint64_t mapu64(uint64_t mini, uint64_t maxi, uint64_t mino, uint64_t maxo, uint64_t value);

extern int powi(int base, unsigned int exp);
extern long powl(long base, unsigned int exp);
extern long long powll(long long base, unsigned int exp);
extern unsigned int powu(unsigned int base, unsigned int exp);
extern unsigned long powul(unsigned long base, unsigned int exp);
extern unsigned long long powull(unsigned long long base, unsigned int exp);
extern int8_t pows8(int8_t base, unsigned int exp);
extern int16_t pows16(int16_t base, unsigned int exp);
extern int32_t pows32(int32_t base, unsigned int exp);
extern int64_t pows64(int64_t base, unsigned int exp);
extern uint8_t powu8(uint8_t base, unsigned int exp);
extern uint16_t powu16(uint16_t base, unsigned int exp);
extern uint32_t powu32(uint32_t base, unsigned int exp);
extern uint64_t powu64(uint64_t base, unsigned int exp);

extern unsigned int sqrti(unsigned int value);
extern unsigned long sqrtl(unsigned long value);
extern unsigned long long sqrtll(unsigned long long value);
extern uint8_t sqrt8(uint8_t value);
extern uint16_t sqrt16(uint16_t value);
extern uint32_t sqrt32(uint32_t value);
extern uint64_t sqrt64(uint64_t value);

#endif /* _MATH_H_ */
