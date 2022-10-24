/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _MACRO_H_
#define _MACRO_H_

#include <stddef.h>

/**
 * VAL_BOOL - convert value to boolean value.
 * @val: value to convert.
 */
#define VAL_BOOL(val) ( \
    (val) ? true : false \
)

/**
 * COND_PTR - Conditional return pointer.
 * @cond: conditions for execution.
 * @ptr: pointer returned when execution is true.
 */
#define COND_PTR(cond, ptr) ( \
    (cond) ? (ptr) : NULL \
)

/**
 * REPEAT_BYTE - repeat the value multiple times to unsigned long.
 * @char: value to repeat.
 */
#define REPEAT_BYTE(char) ( \
    (~0UL / 0xff) * char \
)

/**
 * swap - swap values.
 * @a: first value.
 * @b: second value.
 */
#define swap(a, b) ({ \
    typeof(a) _a = (a); \
    (a) = (b); (b) = _a; \
})

/**
 * upper_32_bits - return bits 32-63 of a number.
 * @val: the value we're accessing.
 */
#define upper_32_bits(val) ( \
    (uint32_t)(((val) >> 16) >> 16) \
)

/**
 * lower_32_bits - return bits 0-31 of a number.
 * @val: the value we're accessing.
 */
#define lower_32_bits(val) ( \
    (uint32_t)((val) & 0xffffffff) \
)

/**
 * upper_16_bits - return bits 16-31 of a number.
 * @val: the value we're accessing.
 */
#define upper_16_bits(val) ( \
    (uint16_t)((val) >> 16) \
)

/**
 * lower_16_bits - return bits 0-15 of a number.
 * @val: the value we're accessing.
 */
#define lower_16_bits(val) ( \
    (uint16_t)((val) & 0xffff) \
)

/* This counts to 12. Any more, it will return 13th argument. */
#define __COUNT_ARGS(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _n, X...) _n
#define COUNT_ARGS(X...) __COUNT_ARGS(, ##X, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0)

#endif  /* _MACRO_H_ */
