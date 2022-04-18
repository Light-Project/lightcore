/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _ASM_CSKY_CMPXCHG_H_
#define _ASM_CSKY_CMPXCHG_H_

#define __cmpxchg_relaxed(ptr, old, val, size)  \
({                                              \
    __typeof__(ptr) __ptr = (ptr);              \
    __typeof__(val) __new = (val);              \
    __typeof__(val) __tmp;                      \
    __typeof__(old) __old = (old);              \
    __typeof__(*(ptr)) __ret;                   \
    switch (size) {                             \
    case 4:                                     \
        asm volatile (                          \
        "1: ldex.w      %0, (%3) \n"            \
        "   cmpne       %0, %4   \n"            \
        "   bt          2f       \n"            \
        "   mov         %1, %2   \n"            \
        "   stex.w      %1, (%3) \n"            \
        "   bez         %1, 1b   \n"            \
        "2:                      \n"            \
        : "=&r" (__ret), "=&r" (__tmp)          \
        : "r" (__new), "r"(__ptr), "r"(__old)   \
        :);                                     \
        break;                                  \
    default:                                    \
        __bad_xchg();                           \
    }                                           \
    __ret;                                      \
})

#define cmpxchg_relaxed(ptr, o, n)      \
        __cmpxchg_relaxed()

#include <asm-generic/cmpxchg.h>

#endif
