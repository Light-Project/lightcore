/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _ASM_X86_BITOPS_H_
#define _ASM_X86_BITOPS_H_

#include <asm/asm.h>
#include <asm/rmwcc.h>
#include <asm/alternative.h>

#define RLONG_ADDR(x)               "m" (*(volatile long *) (x))
#define WBYTE_ADDR(x)               "+m" (*(volatile char *) (x))
#define ADDR(addr)                  RLONG_ADDR(addr)
#define CONST_MASK_ADDR(nr, addr)   WBYTE_ADDR((void *)(addr) + ((nr)>>3))
#define CONST_MASK(nr)              (1 << ((nr) & 7))

#define arch_bit_clr arch_bit_clr
static __always_inline void arch_bit_clr(volatile unsigned long *addr, long bit)
{
    asm volatile (
        __ASM_SIZE(btr) " %1,%0"
        : : ADDR(addr), "Ir" (bit)
        : "memory"
    );
}

#define arch_bit_set arch_bit_set
static __always_inline void arch_bit_set(volatile unsigned long *addr, long bit)
{
    asm volatile (
        __ASM_SIZE(bts) " %1,%0"
        : : ADDR(addr), "Ir" (bit)
        : "memory"
    );
}

#define arch_bit_flip arch_bit_flip
static __always_inline void arch_bit_flip(volatile unsigned long *addr, long bit)
{
    asm volatile (
        __ASM_SIZE(btc) " %1,%0"
        : : ADDR(addr), "Ir" (bit)
        : "memory"
    );
}

#define arch_bit_test arch_bit_test
static __always_inline bool arch_bit_test(volatile unsigned long *addr, long bit)
{
    bool ret;

    asm volatile (
        __ASM_SIZE(bt) " %2,%1"
        CC_SET(c)
        : CC_OUT(c)(ret)
        : ADDR(addr), "Ir"(bit)
        : "memory"
    );

    return ret;
}

#define arch_bit_test_set arch_bit_test_set
static __always_inline bool arch_bit_test_set(volatile unsigned long *addr, long bit)
{
    bool oldbit;

    asm volatile (
        __ASM_SIZE(bts) " %2,%1"
        CC_SET(c)
        : CC_OUT(c) (oldbit)
        : ADDR(addr), "Ir" (bit) : "memory"
    );

    return oldbit;
}

#define arch_bit_test_clr arch_bit_test_clr
static __always_inline bool arch_bit_test_clr(volatile unsigned long *addr, long bit)
{
    bool oldbit;

    asm volatile (
        __ASM_SIZE(btr) " %2,%1"
        CC_SET(c)
        : CC_OUT(c) (oldbit)
        : ADDR(addr), "Ir" (bit)
        : "memory"
    );

    return oldbit;
}

#define arch_bit_test_flip arch_bit_test_flip
static __always_inline bool arch_bit_test_flip(volatile unsigned long *addr, long bit)
{
    bool oldbit;

    asm volatile (
        __ASM_SIZE(btc) " %2,%1"
        CC_SET(c)
        : CC_OUT(c) (oldbit)
        : ADDR(addr), "Ir" (bit)
        : "memory"
    );

    return oldbit;
}

#define arch_bit_atomic_clr arch_bit_atomic_clr
static __always_inline void arch_bit_atomic_clr(volatile unsigned long *addr, long bit)
{
    if (__builtin_constant_p(bit)) {
        asm volatile (
            LOCK_PREFIX "andb %b1,%0"
            : CONST_MASK_ADDR(bit, addr)
            : "iq" (~CONST_MASK(bit))
        );
    } else {
        asm volatile (
            LOCK_PREFIX __ASM_SIZE(btr) " %1,%0"
            :: RLONG_ADDR(addr), "Ir" (bit)
            : "memory"
        );
    }
}

#define arch_bit_atomic_set arch_bit_atomic_set
static __always_inline void arch_bit_atomic_set(volatile unsigned long *addr, long bit)
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
            LOCK_PREFIX __ASM_SIZE(bts) " %1,%0"
            :: RLONG_ADDR(addr), "Ir" (bit)
            : "memory"
        );
    }
}

#define arch_bit_atomic_flip arch_bit_atomic_flip
static __always_inline void arch_bit_atomic_flip(volatile unsigned long *addr, long bit)
{
    if (__builtin_constant_p(bit)) {
        asm volatile(
            LOCK_PREFIX "xorb %b1,%0"
            : CONST_MASK_ADDR(bit, addr)
            : "iq" (CONST_MASK(bit))
        );
    } else {
        asm volatile(
            LOCK_PREFIX __ASM_SIZE(btc) " %1,%0"
            : : RLONG_ADDR(addr), "Ir" (bit)
            : "memory"
        );
    }
}

#define arch_bit_atomic_test arch_bit_atomic_test
static __always_inline bool arch_bit_atomic_test(volatile unsigned long *addr, long bit)
{
    return GEN_BINARY_RMWcc(LOCK_PREFIX __ASM_SIZE(bt), *addr, c, "Ir", bit);
}

#define arch_bit_atomic_test_clr arch_bit_atomic_test_clr
static __always_inline bool arch_bit_atomic_test_clr(volatile unsigned long *addr, long bit)
{
    return GEN_BINARY_RMWcc(LOCK_PREFIX __ASM_SIZE(btr), *addr, c, "Ir", bit);
}

#define arch_bit_atomic_test_set arch_bit_atomic_test_set
static __always_inline bool arch_bit_atomic_test_set(volatile unsigned long *addr, long bit)
{
    return GEN_BINARY_RMWcc(LOCK_PREFIX __ASM_SIZE(bts), *addr, c, "Ir", bit);
}

#define arch_bit_atomic_test_flip arch_bit_atomic_test_flip
static __always_inline bool arch_bit_atomic_test_flip(volatile unsigned long *addr, long bit)
{
    return GEN_BINARY_RMWcc(LOCK_PREFIX __ASM_SIZE(btc), *addr, c, "Ir", bit);
}

/**
 * ffsuf - find first set bit in word (unsafe)
 * @value: The word to search
 * Undefined if no bit exists, so code should check against 0 first.
 */
#define ffsuf ffsuf
static __always_inline unsigned int ffsuf(unsigned long value)
{
    asm volatile (
        "rep            \n"
        "bsf    %1,%0   \n"
        : "=r" (value)
        : "rm" (value)
    );
    return value;
}

/*
 * flsuf: find last set bit in word (unsafe)
 * @value: The word to search
 * Undefined if no set bit exists, so code should check against 0 first.
 */
#define flsuf flsuf
static __always_inline unsigned int flsuf(unsigned long value)
{
    asm volatile (
        "bsr    %1,%0"
        : "=r" (value)
        : "rm" (value)
    );
    return value;
}

/**
 * ffs - find first set bit in word
 * @value: The word to search
 */
#define ffs ffs
static __always_inline unsigned int ffs(unsigned long value)
{
#if defined(CONFIG_ARCH_X86_32)
    asm volatile (
        "bsfl   %1, %0  \n"
        "jnz    1f      \n"
        "movl   $-1, %0 \n"
        "1:"
        : "=r"(value)
        : "rm"(value)
    );
#elif defined(CONFIG_ARCH_X86_64)
    int d0;
    asm volatile (
        "bsfl   %1, %0  \n"
        : "=r"(d0)
        : "rm"(value), "0"(-1)
    );
#endif

    return value + 1;
}

/*
 * fls: find last set bit in word
 * @value: The word to search
 */
#define fls fls
static __always_inline unsigned int fls(unsigned long value)
{
#if defined(CONFIG_ARCH_X86_32)
    asm volatile (
        "bsr    %1, %0  \n"
        "jnz    1f      \n"
        "mov    $-1, %0 \n"
        "1:"
        :"=r"(value)
        :"rm"(value)
    );
#elif defined(CONFIG_ARCH_X86_64)
    int d0;
    asm volatile (
        "bsr    %1, %0  \n"
        : "=r"(d0)
        : "rm"(value), "0"(-1)
    );
#endif

    return value + 1;
}

#include <asm-generic/bitops.h>

#endif  /* _ASM_X86_BITOPS_H_ */
