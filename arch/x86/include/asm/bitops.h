/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _ASM_X86_BITOPS_H_
#define _ASM_X86_BITOPS_H_

#include <asm/asm.h>
#include <asm/alternative.h>

#define RLONG_ADDR(x)               "m" (*(volatile long *) (x))
#define WBYTE_ADDR(x)               "+m" (*(volatile char *) (x))
#define CONST_MASK_ADDR(nr, addr)   WBYTE_ADDR((void *)(addr) + ((nr)>>3))
#define CONST_MASK(nr)              (1 << ((nr) & 7))

#define bit_clr bit_clr
static __always_inline void bit_clr(unsigned long *addr, int bit)
{
    if (__builtin_constant_p(bit)) {
        asm volatile (
            LOCK_PREFIX "andb %b1,%0"
            : CONST_MASK_ADDR(bit, addr)
            : "iq" (~CONST_MASK(bit))
        );
    } else {
        asm volatile (
            LOCK_PREFIX __ASM_SIZE(btr) "%1,%0"
            :: RLONG_ADDR(addr), "Ir" (bit)
            : "memory"
        );
    }
}

#define bit_set bit_set
static __always_inline void bit_set(unsigned long *addr, int bit)
{
    if (__builtin_constant_p(bit)) {
        asm volatile (
            LOCK_PREFIX "orb %b1,%0"
            : CONST_MASK_ADDR(bit, addr)
            : "iq" (CONST_MASK(bit))
            : "memory"
        );
    } else {
        asm volatile (
            LOCK_PREFIX __ASM_SIZE(bts) "%1,%0"
            :: RLONG_ADDR(addr), "Ir" (bit)
            : "memory"
        );
    }
}

#define bit_test bit_test
static __always_inline bool bit_test(unsigned long *addr, int bit)
{
    bool ret;

    asm volatile(
        __ASM_SIZE(bt) " %2,%1" CC_SET(c)
        : CC_OUT(c)(ret)
        : "m"(*(unsigned long *)addr), "Ir"(bit)
        : "memory"
    );

    return ret;
}

#define ffs ffs
static __always_inline int ffs(unsigned long val)
{
    asm volatile (
        "bsfl   %1,%0\n"
        "jnz    1f\n"
        "movl   $-1,%0\n"
        "1:"
        : "=r"(val)
        : "rm"(val)
    );

    return val;
}

#define fls fls
static __always_inline int fls(unsigned long val)
{
    asm volatile (
        "bsrl   %1,%0\n\t"
        "jnz    1f\n\t"
        "movl   $-1,%0\n"
        "1:"
        : "=r"(val)
        : "rm"(val)
    );

    return val;
}

#include <asm-generic/bitops.h>

#endif  /* _ASM_X86_BITOPS_H_ */
