/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _MACRO_H_
#define _MACRO_H_

#include <stddef.h>

/**
 * VAL_BOOL - convert value to boolean value.
 * @val: value to convert.
 */
#define VAL_BOOL(val) (         \
    (val) ? true : false        \
)

/**
 * COND_PTR - Conditional return pointer.
 * @cond: conditions for execution.
 * @ptr: pointer returned when execution is true.
 */
#define COND_PTR(cond, ptr) (   \
    (cond) ? (ptr) : NULL       \
)

/**
 * REPEAT_BYTE - repeat the value multiple times to unsigned long
 * @char: value to repeat
 */
#define REPEAT_BYTE(char) (     \
    (~0UL / 0xff) * char        \
)

/**
 * swap - swap values
 * @a: first value
 * @b: second value
 */
#define swap(a, b) ({           \
    typeof(a) _a = (a);         \
    (a) = (b);                  \
    (b) = _a;                   \
})

/**
 * skzalloc - static alloc zeroed memory in stack
 * @size: memory size to alloc
 */
#define skzalloc(size) (        \
    (void *)(char [size]){}     \
)

#endif  /* _MACRO_H_ */
