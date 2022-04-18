/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _ASM_X86_ATOMIC_H
#define _ASM_X86_ATOMIC_H

#include <types.h>
#include <asm/asm.h>
#include <asm/alternative.h>
#include <asm/cmpxchg.h>
#include <asm/rmwcc.h>

/**
 * arch_atomic_read - read atomic variable
 * @atomic: pointer of type atomic_t
 */
#define arch_atomic_read arch_atomic_read
static __always_inline atomic_t arch_atomic_read(atomic_t *atomic)
{
    return READ_ONCE(*atomic);
}

/**
 * arch_atomic_write - write atomic variable
 * @atomic: pointer of type atomic_t
 * @val: required value
 */
#define arch_atomic_write arch_atomic_write
static __always_inline void arch_atomic_write(atomic_t *atomic, atomic_t val)
{
    WRITE_ONCE(*atomic, val);
}

#define arch_atomic_xchg arch_atomic_xchg
static __always_inline atomic_t arch_atomic_xchg(atomic_t *atomic, atomic_t val)
{
    return arch_xchg(atomic, val);
}
/**
 * arch_atomic_cmpxchg - cmp and xchg atomic variable
 * @atomic: pointer of type atomic_t
 * @val: required value
 */
#define arch_atomic_cmpxchg arch_atomic_cmpxchg
static __always_inline atomic_t arch_atomic_cmpxchg(atomic_t *atomic, atomic_t old, atomic_t val)
{
    return arch_cmpxchg(atomic, old, val);
}

#define arch_atomic_try_cmpxchg arch_atomic_try_cmpxchg
static __always_inline bool arch_atomic_try_cmpxchg(atomic_t *atomic, atomic_t *old, atomic_t val)
{
    return arch_try_cmpxchg(atomic, old, val);
}

/**
 * arch_atomic_add - add integer to atomic variable
 * @atomic: pointer of type atomic_t
 * @val: required value
 */
#define arch_atomic_add arch_atomic_add
static __always_inline void arch_atomic_add(atomic_t *atomic, atomic_t val)
{
    asm volatile (
        LOCK_PREFIX _ASM_ADD "%1,%0"
        : "+m"(atomic)
        : "ir"(val)
        : "memory"
    );
}

/**
 * arch_atomic_sub - subtract integer from atomic variable
 * @atomic: pointer of type atomic_t
 * @val: required value
 */
#define arch_atomic_sub arch_atomic_sub
static __always_inline void arch_atomic_sub(atomic_t *atomic, atomic_t val)
{
    asm volatile (
        LOCK_PREFIX _ASM_SUB "%1, %0"
        : "+m"(atomic)
        : "ir"(val)
        : "memory"
    );
}

/**
 * arch_atomic_inc - increment atomic variable
 * @atomic: pointer of type atomic_t
 */
#define arch_atomic_inc arch_atomic_inc
static __always_inline void arch_atomic_inc(atomic_t *atomic)
{
    asm volatile (
        LOCK_PREFIX _ASM_INC "%0"
        : "+m"(atomic)
        :: "memory"
    );
}

/**
 * arch_atomic_dec - decrement atomic variable
 * @atomic: pointer of type atomic_t
 */
#define arch_atomic_dec arch_atomic_dec
static __always_inline void arch_atomic_dec(atomic_t *atomic)
{
    asm volatile (
        LOCK_PREFIX _ASM_DEC "%0"
        : "+m"(atomic)
        :: "memory"
    );
}

/**
 * arch_atomic_and - and atomic variable
 * @atomic: pointer of type atomic_t
 * @val: required value
 */
#define arch_atomic_and arch_atomic_and
static __always_inline void arch_atomic_and(atomic_t *atomic, atomic_t val)
{
    asm volatile (
        LOCK_PREFIX _ASM_AND "%1, %0"
        : "+m" (atomic)
        : "ir" (val)
        : "memory"
    );
}

/**
 * arch_atomic_or - or atomic variable
 * @atomic: pointer of type atomic_t
 * @val: required value
 */
#define arch_atomic_or arch_atomic_or
static __always_inline void arch_atomic_or(atomic_t *atomic, atomic_t val)
{
    asm volatile (
        LOCK_PREFIX _ASM_OR "%1, %0"
        : "+m"(atomic)
        : "ir"(val)
        : "memory"
    );
}

/**
 * arch_atomic_xor - xor atomic variable
 * @atomic: pointer of type atomic_t
 * @val: required value
 */
#define arch_atomic_xor arch_atomic_xor
static __always_inline void arch_atomic_xor(atomic_t *atomic, atomic_t val)
{
    asm volatile (
        LOCK_PREFIX _ASM_XOR "%1, %0"
        : "+m"(atomic)
        : "ir"(val)
        : "memory"
    );
}

/**
 * arch_atomic_fetch_add - fetch and atomic add
 * @atomic: pointer of type atomic_t
 * @val: required value
 */
#define arch_atomic_fetch_add arch_atomic_fetch_add
static __always_inline atomic_t arch_atomic_fetch_add(atomic_t *atomic, atomic_t val)
{
    return arch_xadd(atomic, val);
}

/**
 * arch_atomic_fetch_sub - fetch and atomic sub
 * @atomic: pointer of type atomic_t
 * @val: required value
 */
#define arch_atomic_fetch_sub arch_atomic_fetch_sub
static __always_inline atomic_t arch_atomic_fetch_sub(atomic_t *atomic, atomic_t val)
{
    return arch_xadd(atomic, -val);
}

/**
 * arch_atomic_fetch_and - fetch and atomic and
 * @atomic: pointer of type atomic_t
 * @val: required value
 */
#define arch_atomic_fetch_and arch_atomic_fetch_and
static __always_inline atomic_t arch_atomic_fetch_and(atomic_t *atomic, atomic_t val)
{
    atomic_t old = arch_atomic_read(atomic);
    do {} while (!arch_atomic_try_cmpxchg(atomic, &old, old & val));
    return old;
}

/**
 * arch_atomic_fetch_or - fetch and atomic or
 * @atomic: pointer of type atomic_t
 * @val: required value
 */
#define arch_atomic_fetch_or arch_atomic_fetch_or
static __always_inline atomic_t arch_atomic_fetch_or(atomic_t *atomic, atomic_t val)
{
    atomic_t old = arch_atomic_read(atomic);
    do {} while (!arch_atomic_try_cmpxchg(atomic, &old, old | val));
    return old;
}

/**
 * arch_atomic_fetch_xor - fetch and atomic xor
 * @atomic: pointer of type atomic_t
 * @val: required value
 */
#define arch_atomic_fetch_xor arch_atomic_fetch_xor
static __always_inline atomic_t arch_atomic_fetch_xor(atomic_t *atomic, atomic_t val)
{
    atomic_t old = arch_atomic_read(atomic);
    do {} while (!arch_atomic_try_cmpxchg(atomic, &old, old ^ val));
    return old;
}

/**
 * arch_atomic_sub_test - subtract value from variable and test result
 * @atomic: pointer of type atomic_t
 * @val: required value
 */
#define arch_atomic_sub_test arch_atomic_sub_test
static __always_inline bool arch_atomic_sub_test(atomic_t *atomic, atomic_t val)
{
    return GEN_BINARY_RMWcc(LOCK_PREFIX _ASM_SUB, atomic, e, "er", val);
}

/**
 * arch_atomic_inc_test - increment and test result
 * @atomic: pointer of type atomic_t
 */
#define arch_atomic_inc_test arch_atomic_inc_test
static __always_inline bool arch_atomic_inc_test(atomic_t *atomic)
{
    return GEN_UNARY_RMWcc(LOCK_PREFIX _ASM_INC, atomic, e);
}

/**
 * arch_atomic_dec_test - decrement and test result
 * @atomic: pointer of type atomic_t
 */
#define arch_atomic_dec_test arch_atomic_dec_test
static __always_inline bool arch_atomic_dec_test(atomic_t *atomic)
{
    return GEN_UNARY_RMWcc(LOCK_PREFIX _ASM_DEC, atomic, e);
}

#include <asm-generic/atomic.h>

#endif /* _ASM_X86_ATOMIC_H */
