/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _ASM_X86_ATOMIC_H
#define _ASM_X86_ATOMIC_H

#include <types.h>
#include <asm/asm.h>
#include <asm/rwonce.h>
#include <asm/alternative.h>
#include <asm/cmpxchg.h>
#include <asm/rmwcc.h>

/**
 * atomic_read - read atomic variable
 * @atomic: pointer of type atomic_t
 */
#define atomic_read atomic_read
static __always_inline atomic_t atomic_read(const atomic_t *atomic)
{
    return READ_ONCE(*atomic);
}

/**
 * atomic_write - write atomic variable
 * @atomic: pointer of type atomic_t
 * @val: required value
 */
#define atomic_write atomic_write
static __always_inline void atomic_write(atomic_t *atomic, atomic_t val)
{
    WRITE_ONCE(*atomic, val);
}

/**
 * atomic_add - add integer to atomic variable
 * @atomic: pointer of type atomic_t
 * @val: required value
 */
#define atomic_add atomic_add
static __always_inline void atomic_add(atomic_t *atomic, atomic_t val)
{
    asm volatile (
        LOCK_PREFIX "addl %1,%0"
        :"+m" (atomic)
        :"ir" (val) :"memory"
    );
}

/**
 * atomic_sub - subtract integer from atomic variable
 * @atomic: pointer of type atomic_t
 * @val: required value
 */
#define atomic_sub atomic_sub
static __always_inline void atomic_sub(atomic_t *atomic, atomic_t val)
{
    asm volatile (
        LOCK_PREFIX "subl %1,%0"
        :"+m" (atomic)
        :"ir"(val) :"memory"
    );
}

/**
 * atomic_inc - increment atomic variable
 * @atomic: pointer of type atomic_t
 */
#define atomic_inc atomic_inc
static __always_inline void atomic_inc(atomic_t *atomic)
{
    asm volatile (
        LOCK_PREFIX "incl %0"
        :"+m" (atomic) ::"memory"
    );
}

/**
 * atomic_dec - decrement atomic variable
 * @atomic: pointer of type atomic_t
 */
#define atomic_dec atomic_dec
static __always_inline void atomic_dec(atomic_t *atomic)
{
    asm volatile (
        LOCK_PREFIX "decl %0"
        :"+m" (atomic) ::"memory"
    );
}

/**
 * atomic_and - and atomic variable
 * @atomic: pointer of type atomic_t
 * @val: required value
 */
#define atomic_and atomic_and
static __always_inline void atomic_and(atomic_t *atomic, atomic_t val)
{
    asm volatile (
        LOCK_PREFIX "andl %1,%0"
        :"+m" (atomic)
        :"ir" (val) :"memory"
    );
}

/**
 * atomic_or - or atomic variable
 * @atomic: pointer of type atomic_t
 * @val: required value
 */
#define atomic_or atomic_or
static __always_inline void atomic_or(atomic_t *atomic, atomic_t val)
{
    asm volatile (
        LOCK_PREFIX "orl %1,%0"
        :"+m"(atomic)
        :"ir"(val) :"memory"
    );
}

/**
 * atomic_xor - xor atomic variable
 * @atomic: pointer of type atomic_t
 * @val: required value
 */
#define atomic_xor atomic_xor
static __always_inline void atomic_xor(atomic_t *atomic, atomic_t val)
{
    asm volatile (
        LOCK_PREFIX "xorl %1,%0"
        :"+m" (atomic)
        :"ir"(val) :"memory"
    );
}

/**
 * atomic_xchg - xchg atomic variable
 * @atomic: pointer of type atomic_t
 */
#define atomic_xchg atomic_xchg
static __always_inline atomic_t atomic_xchg(atomic_t *atomic, atomic_t new)
{
    return xchg(atomic, new);
}

/**
 * atomic_cmpxchg - cmp and xchg atomic variable
 * @atomic: pointer of type atomic_t
 */
#define atomic_cmpxchg atomic_cmpxchg
static __always_inline atomic_t atomic_cmpxchg(atomic_t *atomic, atomic_t old, atomic_t new)
{
    return cmpxchg(atomic, old, new);
}

/**
 * atomic_try_cmpxchg -
 * @atomic: pointer of type atomic_t
 */
#define atomic_try_cmpxchg atomic_try_cmpxchg
static __always_inline atomic_t atomic_try_cmpxchg(atomic_t *atomic, atomic_t *oldp, atomic_t new)
{
    return try_cmpxchg(atomic, oldp, new);
}

/**
 * atomic_fetch_add - fetch and atomic add
 * @atomic: pointer of type atomic_t
 * @val: required value
 */
#define atomic_fetch_add atomic_fetch_add
static __always_inline atomic_t atomic_fetch_add(atomic_t *atomic, atomic_t val)
{
    return xadd(atomic, val);
}

/**
 * atomic_fetch_sub - fetch and atomic sub
 * @atomic: pointer of type atomic_t
 * @val: required value
 */
#define atomic_fetch_sub atomic_fetch_sub
static __always_inline atomic_t atomic_fetch_sub(atomic_t *atomic, atomic_t val)
{
    return xadd(atomic, -val);
}

/**
 * atomic_fetch_and - fetch and atomic and
 * @atomic: pointer of type atomic_t
 * @val: required value
 */
#define atomic_fetch_and atomic_fetch_and
static __always_inline atomic_t atomic_fetch_and(atomic_t *atomic, atomic_t val)
{
    atomic_t old = atomic_read(atomic);

    do { } while (!atomic_try_cmpxchg(atomic, &old, old & val));

    return old;
}

/**
 * atomic_fetch_or - fetch and atomic or
 * @atomic: pointer of type atomic_t
 * @val: required value
 */
#define atomic_fetch_or atomic_fetch_or
static __always_inline atomic_t atomic_fetch_or(atomic_t *atomic, atomic_t val)
{
    atomic_t old = atomic_read(atomic);

    do { } while (!atomic_try_cmpxchg(atomic, &old, old | val));

    return old;
}

/**
 * atomic_fetch_xor - fetch and atomic xor
 * @atomic: pointer of type atomic_t
 * @val: required value
 */
#define atomic_fetch_xor atomic_fetch_xor
static __always_inline atomic_t atomic_fetch_xor(atomic_t *atomic, atomic_t val)
{
    atomic_t old = atomic_read(atomic);

    do { } while (!atomic_try_cmpxchg(atomic, &old, old ^ val));

    return old;
}

/**
 * atomic_sub_test - subtract value from variable and test result
 * @atomic: pointer of type atomic_t
 * @val: required value
 */
#define atomic_sub_test atomic_sub_test
static __always_inline bool atomic_sub_test(atomic_t *atomic, atomic_t val)
{
    return GEN_BINARY_RMWcc(LOCK_PREFIX "subl", atomic, e, "er", val);
}

/**
 * atomic_inc_test - increment and test result
 * @atomic: pointer of type atomic_t
 */
#define atomic_inc_test atomic_inc_test
static __always_inline bool atomic_inc_test(atomic_t *atomic)
{
    return GEN_UNARY_RMWcc(LOCK_PREFIX "incl", atomic, e);
}

/**
 * atomic_dec_test - decrement and test result
 * @atomic: pointer of type atomic_t
 */
#define atomic_dec_test atomic_dec_test
static __always_inline bool atomic_dec_test(atomic_t *atomic)
{
    return GEN_UNARY_RMWcc(LOCK_PREFIX "decl", atomic, e);
}

#endif /* _ASM_X86_ATOMIC_H */
