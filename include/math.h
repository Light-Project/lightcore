/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _MATH_H_
#define _MATH_H_

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

#endif  /* _MATH_H_ */
