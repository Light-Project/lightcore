/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _KERNEL_H_
#define _KERNEL_H_

#include <types.h>
#include <limits.h>
#include <stddef.h>
#include <stdarg.h>
#include <error.h>
#include <state.h>
#include <size.h>
#include <math.h>
#include <log2.h>

#define KMAGIC  0x4c434d4bU
#define MSIZE   (sizeof(void *))

#define REPEAT_BYTE(char) (                             \
    (~0UL / 0xff) * char                                \
)

/**
 * ARRAY_SIZE - get the number of elements in array.
 * @arr: array to be sized.
 */
#define ARRAY_SIZE(arr) (                               \
    sizeof(arr) / sizeof((arr)[0])                      \
)

/**
 * swap - swap values
 * @a: first value
 * @b: second value
 */
#define swap(a, b) ({                                   \
    typeof(a) _a = (a);                                 \
    (a) = (b);                                          \
    (b) = _a;                                           \
})

/**
 * skzalloc - static alloc zeroed memory in stack
 * @size: memory size to alloc
 */
#define skzalloc(size) (                                \
    (void *)(char [size]){}                             \
)

/**
 * min - return minimum of two values of the same or compatible types
 * @a: first value
 * @b: second value
 */
#define min(a, b) ({                                    \
    typeof(a) _amin = (a);                              \
    typeof(a) _bmin = (b);                              \
    (void)(&_amin == &_bmin);                           \
    _amin < _bmin ? _amin : _bmin;                      \
})

/**
 * max - return maximum of two values of the same or compatible types
 * @a: first value
 * @b: second value
 */
#define max(a, b) ({                                    \
    typeof(a) _amax = (a);                              \
    typeof(a) _bmax = (b);                              \
    (void)(&_amax == &_bmax);                           \
    _amax > _bmax ? _amax : _bmax;                      \
})

/**
 * clamp - return a value clamped to a given range with strict typechecking.
 * @val: current value.
 * @lo: lowest allowable value.
 * @hi: highest allowable value.
 */
#define clamp(val, lo, hi) (                            \
    min(max(val, lo), hi)                               \
)

/**
 * upper_32_bits - return bits 32-63 of a number
 * @val: the value we're accessing
 */
#define upper_32_bits(val) (                            \
    (uint32_t)(((val) >> 16) >> 16)                     \
)

/**
 * lower_32_bits - return bits 0-31 of a number
 * @val: the value we're accessing
 */
#define lower_32_bits(val) (                            \
    (u32)((val) & 0xffffffff)                           \
)

/**
 * upper_16_bits - return bits 16-31 of a number
 * @val: the value we're accessing
 */
#define upper_16_bits(val) (                            \
    (uint16_t)((val) >> 16)                             \
)

/**
 * lower_16_bits - return bits 0-15 of a number
 * @val: the value we're accessing
 */
#define lower_16_bits(val) (                            \
    (uint16_t)((val) & 0xffff)                          \
)

/**
 * align_low/high - aligned address.
 * @size: address to aligned.
 * @align: alignment size.
 */
#define align_low(size, align) ({                       \
    (size) & ~((align) - 1);                            \
})

#define align_high(size, align) ({                      \
    typeof(align) _align = (align);                     \
    ((size) + (_align - 1)) & ~(_align - 1);            \
})

#define align_check(size, align) (                      \
    !(size & ((align) - 1))                             \
)

#define align_ptr_low(ptr, align) ({                    \
    (typeof(ptr))align_low((size_t)(ptr), align);       \
})

#define align_ptr_high(ptr, align) ({                   \
    (typeof(ptr))align_high((size_t)(ptr), align);      \
})

#define align_ptr_check(ptr, align) ({                  \
    align_check((size_t)(ptr), align);                  \
})

/**
 * container_of - cast a member of a structure out to the containing structure.
 * @ptr: the pointer to the member.
 * @type: the type of the container struct this is embedded in.
 * @member: the name of the member within the struct.
 */
#define container_of(ptr, type, member) ({              \
    const typeof(((type *)0)->member) *__mptr = (ptr);  \
    (type *)((char *)__mptr - offsetof(type,member));   \
})

#define container_of_safe(ptr, type, member) ({         \
    typeof(ptr) _ptr = (ptr);                           \
    _ptr ? container_of(_ptr, type, member) : NULL;     \
})

extern int atoi(const char *nptr);
extern unsigned long strtoul(const char *nptr);
extern unsigned long long strtoull(const char *nptr);

extern __printf(2, 3) int sprintf(char *buf, const char *fmt, ...);
extern __printf(3, 4) int snprintf(char *buf, size_t n, const char *fmt, ...);
extern __printf(3, 4) int scnprintf(char *buf, size_t n, const char *fmt, ...);
extern int vsprintf(char *buf, const char *fmt, va_list args);
extern int vsnprintf(char *buf, size_t n, const char *fmt, va_list args);
extern int vscnprintf(char *buf, size_t n, const char *fmt, va_list args);
extern int gsize(char *buff, size_t size);

#endif  /* _KERNEL_H_ */
