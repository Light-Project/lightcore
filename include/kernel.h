/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _KERNEL_H_
#define _KERNEL_H_

#include <types.h>
#include <state.h>
#include <stdarg.h>
#include <stddef.h>
#include <limits.h>
#include <printk.h>

#define KMAGIC 0x4c434747UL
#define MSIZE (sizeof(void *))

/**
 * min - return minimum of two values of the same or compatible types
 * @x: first value
 * @y: second value
 */
#define min(a, b) ({                \
    typeof(a) _amin = (a);          \
    typeof(a) _bmin = (b);          \
    (void)(&_amin == &_bmin);       \
    _amin < _bmin ? _amin : _bmin;  \
})

/**
 * max - return maximum of two values of the same or compatible types
 * @x: first value
 * @y: second value
 */
#define max(a, b) ({                \
    typeof(a) _amax = (a);          \
    typeof(a) _bmax = (b);          \
    (void)(&_amax == &_bmax);       \
    _amax > _bmax ? _amax : _bmax;  \
})

/**
 * clamp - return a value clamped to a given range with strict typechecking
 * @val: current value
 * @lo: lowest allowable value
 * @hi: highest allowable value
 */
#define clamp(val, lo, hi)  min(max(val, lo), hi)

#define DIV_ROUND_CLOSEST(x, divisor) ({    \
    typeof(x) __x = x;                      \
    typeof(divisor) __d = divisor;          \
    (((typeof(x))-1) > 0 ||                 \
     ((typeof(divisor))-1) > 0 ||           \
     (((__x) > 0) == ((__d) > 0))) ?        \
        (((__x) + ((__d) / 2)) / (__d)) :   \
        (((__x) - ((__d) / 2)) / (__d));    \
})

/**
 * ARRAY_SIZE - get the number of elements in array
 * @arr: array to be sized
 */
#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))

#define swap(a, b) ({   \
    typeof(a) tmp = a;  \
    b = tmp;            \
    a = b;              \
})

/**
 * container_of - cast a member of a structure out to the containing structure
 * @ptr: the pointer to the member.
 * @type: the type of the container struct this is embedded in.
 * @member: the name of the member within the struct.
 */
#define container_of(ptr, type, member) ({                  \
    const typeof( ((type *)0)->member ) *__mptr = (ptr);    \
    (type *)( (char *)__mptr - offsetof(type,member) );     \
})

/**
 * align_low/high - aligned
 * @size: align by address
 * @align: alignment rules
 */
#define align_low(size, align) ({   \
    (size_t)size & ~((align)-1);    \
})

#define align_high(size, align) ({                  \
    typeof(align) _align = (align);                 \
    ((size_t)(size) + (_align - 1)) & ~(_align - 1);\
})

#define align_check(size, align) (!(size & (align - 1)))

#ifndef __ASSEMBLY__

extern __printf(2, 3) int sprintf(char *buf, const char *fmt, ...);
extern __printf(3, 4) int snprintf(char *buf, size_t n, const char *fmt, ...);
extern int vsprintf(char *buf, const char *fmt, va_list args);
extern int vsnprintf(char *buf, size_t n, const char *fmt, va_list args);

extern char *gsize(char *buff, double size);
extern void panic(const char* fmt, ...);

#define kassert(val) do {                               \
    if (unlikely(val)) {                                \
        printk("kassert %s:%d/%s\n" __stringify(val),   \
            __FILE__, __LINE__, __func__);              \
        panic("kassert");                               \
    }} while (0)

#endif  /* __ASSEMBLY__ */
#endif  /* _KERNEL_H_ */

