/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _ASM_X86_CMPXCHG_H_
#define _ASM_X86_CMPXCHG_H_

#include <types.h>
#include <asm/asm.h>

#define __xchg_op(op, ptr, val, lock) ({                        \
    typeof (*(ptr)) __ret = (val);                              \
    switch (sizeof(*(ptr))) {                                   \
        case 1:                                                 \
            asm volatile (lock #op "b %b0, %1\n"                \
                    : "+q" (__ret), "+m" (*(ptr))               \
                    : : "memory", "cc");                        \
            break;                                              \
        case 2:                                                 \
            asm volatile (lock #op "w %w0, %1\n"                \
                    : "+r" (__ret), "+m" (*(ptr))               \
                    : : "memory", "cc");                        \
            break;                                              \
        case 4:                                                 \
            asm volatile (lock #op "l %0, %1\n"                 \
                    : "+r" (__ret), "+m" (*(ptr))               \
                    : : "memory", "cc");                        \
            break;                                              \
        case 8: default:                                        \
            asm volatile (lock #op "q %q0, %1\n"                \
                    : "+r" (__ret), "+m" (*(ptr))               \
                    : : "memory", "cc");                        \
    }                                                           \
    __ret;                                                      \
})

#define __cmpxchg(ptr, old, new, lock) ({                       \
    typeof(*(ptr)) __new = (new);                               \
    typeof(*(ptr)) __old = (old);                               \
    typeof(*(ptr)) __ret;                                       \
    switch (sizeof(*(ptr))) {                                   \
        case 1: {                                               \
            volatile u8 *__ptr = (volatile u8 *)(ptr);          \
            asm volatile(lock "cmpxchgb %2,%1"                  \
                    : "=a" (__ret), "+m" (*__ptr)               \
                    : "q" (__new), "0" (__old)                  \
                    : "memory");                                \
            break;                                              \
        }                                                       \
        case 2: {                                               \
            volatile u16 *__ptr = (volatile u16 *)(ptr);        \
            asm volatile(lock "cmpxchgw %2,%1"                  \
                    : "=a" (__ret), "+m" (*__ptr)               \
                    : "r" (__new), "0" (__old)                  \
                    : "memory");                                \
            break;                                              \
        }                                                       \
        case 4: {                                               \
            volatile u32 *__ptr = (volatile u32 *)(ptr);        \
            asm volatile(lock "cmpxchgl %2,%1"                  \
                    : "=a" (__ret), "+m" (*__ptr)               \
                    : "r" (__new), "0" (__old)                  \
                    : "memory");                                \
            break;                                              \
        }                                                       \
        case 8: default: {                                      \
            volatile u64 *__ptr = (volatile u64 *)(ptr);        \
            asm volatile(lock "cmpxchgq %2,%1"                  \
                    : "=a" (__ret), "+m" (*__ptr)               \
                    : "r" (__new), "0" (__old)                  \
                    : "memory");                                \
            break;                                              \
        }                                                       \
    }                                                           \
    __ret;                                                      \
})

#define __try_cmpxchg(_ptr, _pold, _new, lock) ({               \
    typeof(_ptr) _old = (__typeof__(_ptr))(_pold);              \
    typeof(*(_ptr)) __old = *_old;                              \
    typeof(*(_ptr)) __new = (_new);                             \
    bool status;                                                \
    switch (sizeof(*(_ptr))) {                                  \
        case 1: {                                               \
            volatile u8 *__ptr = (volatile u8 *)(_ptr);         \
            asm volatile(lock "cmpxchgb %[new], %[ptr]"         \
                    CC_SET(z)                                   \
                    : CC_OUT(z) (status),                       \
                    [ptr] "+m" (*__ptr),                        \
                    [old] "+a" (__old)                          \
                    : [new] "q" (__new)                         \
                    : "memory");                                \
            break;                                              \
        }                                                       \
        case 2: {                                               \
            volatile u16 *__ptr = (volatile u16 *)(_ptr);       \
            asm volatile(lock "cmpxchgw %[new], %[ptr]"         \
                    CC_SET(z)                                   \
                    : CC_OUT(z) (status),                       \
                    [ptr] "+m" (*__ptr),                        \
                    [old] "+a" (__old)                          \
                    : [new] "r" (__new)                         \
                    : "memory");                                \
            break;                                              \
        }                                                       \
        case 4: {                                               \
            volatile u32 *__ptr = (volatile u32 *)(_ptr);       \
            asm volatile(lock "cmpxchgl %[new], %[ptr]"         \
                    CC_SET(z)                                   \
                    : CC_OUT(z) (status),                       \
                    [ptr] "+m" (*__ptr),                        \
                    [old] "+a" (__old)                          \
                    : [new] "r" (__new)                         \
                    : "memory");                                \
            break;                                              \
        }                                                       \
        case 8: default: {                                      \
            volatile u64 *__ptr = (volatile u64 *)(_ptr);       \
            asm volatile(lock "cmpxchgq %[new], %[ptr]"         \
                    CC_SET(z)                                   \
                    : CC_OUT(z) (status),                       \
                    [ptr] "+m" (*__ptr),                        \
                    [old] "+a" (__old)                          \
                    : [new] "r" (__new)                         \
                    : "memory");                                \
            break;                                              \
        }                                                       \
    }                                                           \
    if (unlikely(!status))                                      \
        *_old = __old;                                          \
    likely(status);                                             \
})

#define xchg(ptr, val) __xchg_op(xchg, ptr, val, "")
#define xadd(ptr, val) __xchg_op(xadd, ptr, val, LOCK_PREFIX)

#define cmpxchg(ptr, old, new) __cmpxchg(ptr, old, new, LOCK_PREFIX)
#define cmpxchg_sync(ptr, old, new) __cmpxchg(ptr, old, new, "lock; ")
#define cmpxchg_local(ptr, old, new) __cmpxchg(ptr, old, new, "")

#define try_cmpxchg(ptr, pold, new) __try_cmpxchg(ptr, pold, new, LOCK_PREFIX)

#endif  /* _ASM_X86_CMPXCHG_H_ */
