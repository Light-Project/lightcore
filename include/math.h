/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _MATH_H_
#define _MATH_H_

#include <types.h>

/*
 * This looks more complex than it should be. But we need to
 * get the type for the ~ right in round_down (it needs to be
 * as wide as the result!), and we want to evaluate the macro
 * arguments just once each.
 */
#define round_mask(x, y) (                              \
    (typeof(x))((y) - 1)                                \
)

/**
 * round_up - round up to next specified power of 2
 * @x: the value to round
 * @y: multiple to round up to (must be a power of 2)
 */
#define round_up(x, y) (                                \
    (((x) - 1) | round_mask(x, y)) + 1                  \
)

/**
 * round_down - round down to next specified power of 2
 * @x: the value to round
 * @y: multiple to round down to (must be a power of 2)
 */
#define round_down(x, y) (                              \
    (x) & ~round_mask(x, y)                             \
)

/**
 * DIV_ROUND_UP -
 * @n:
 * @d:
 */
#define DIV_ROUND_UP(n, d) (                            \
    ((n) + (d) - 1) / (d)                               \
)

/*
 * Divide positive or negative dividend by positive or negative divisor
 * and round to closest integer. Result is undefined for negative
 * divisors if the dividend variable type is unsigned and for negative
 * dividends if the divisor variable type is unsigned.
 */
#define DIV_ROUND_CLOSEST(x, divisor) ({                \
    typeof(x) __x = x;                                  \
    typeof(divisor) __d = divisor;                      \
    (((typeof(x)) -1) > 0 ||                            \
     ((typeof(divisor)) -1) > 0 ||                      \
     (((__x) > 0) == ((__d) > 0))) ?                    \
        (((__x) + ((__d) / 2)) / (__d)) :               \
        (((__x) - ((__d) / 2)) / (__d));                \
})

#ifdef TYPE_HAS_INT128

static inline uint64_t mul_u64_u32_shr(uint64_t a, uint32_t mul, unsigned int shift)
{
    return (uint64_t)(((uint128_t)a * mul) >> shift);
}

#else /* !TYPE_HAS_INT128 */

static inline uint64_t mul_u64_u32_shr(uint64_t a, uint32_t mul, unsigned int shift)
{
    uint32_t ah, al;
    uint64_t ret;

    al = a;
    ah = a >> 32;

    ret = (al * mul) >> shift;
    if (ah)
        ret += (ah * mul) << (32 - shift);

    return ret;
}

#endif  /* TYPE_HAS_INT128 */

extern unsigned int gcd(unsigned int first, unsigned int second);
extern unsigned long gcdl(unsigned long first, unsigned long second);
extern unsigned long long gcdll(unsigned long long first, unsigned long long second);
extern uint8_t gcd8(uint8_t first, uint8_t second);
extern uint16_t gcd16(uint16_t first, uint16_t second);
extern uint32_t gcd32(uint32_t first, uint32_t second);
extern uint64_t gcd64(uint64_t first, uint64_t second);

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

#endif  /* _MATH_H_ */
