/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _ASM_X86_ATOMIC_H
#define _ASM_X86_ATOMIC_H

#include <types.h>
#include <asm/asm.h>
#include <asm/rwonce.h>
#include <asm/alternative.h>
#include <asm/rmwcc.h>

#define atomic_read atomic_read
static __always_inline atomic_t atomic_read(const atomic_t *atomic)
{
    return READ_ONCE(*atomic);
}

#define atomic_write atomic_write
static __always_inline void atomic_write(atomic_t *atomic, atomic_t val)
{
    WRITE_ONCE(*atomic, val);
}

#define atomic_add atomic_add
static __always_inline void atomic_add(atomic_t *atomic, atomic_t val)
{
    asm volatile (
        LOCK_PREFIX "addl %1,%0"
        :"+m" (atomic)
        :"ir" (val) :"memory"
    );
}

#define atomic_sub atomic_sub
static __always_inline void atomic_sub(atomic_t *atomic, atomic_t val)
{
    asm volatile (
        LOCK_PREFIX "subl %1,%0"
        :"+m" (atomic)
        :"ir"(val) :"memory"
    );
}

#define atomic_inc atomic_inc
static __always_inline void atomic_inc(atomic_t *atomic)
{
    asm volatile (
        LOCK_PREFIX "incl %0"
        :"+m" (atomic) ::"memory"
    );
}

#define atomic_dec atomic_dec
static __always_inline void atomic_dec(atomic_t *atomic)
{
    asm volatile (
        LOCK_PREFIX "decl %0"
        :"+m" (atomic) ::"memory"
    );
}

#define atomic_and atomic_and
static __always_inline void atomic_and(atomic_t *atomic, atomic_t val)
{
    asm volatile (
        LOCK_PREFIX "andl %1,%0"
        :"+m" (atomic)
        :"ir" (val) :"memory"
    );
}

#define atomic_or atomic_or
static __always_inline void atomic_or(atomic_t *atomic, atomic_t val)
{
    asm volatile (
        LOCK_PREFIX "orl %1,%0"
        :"+m"(atomic)
        :"ir"(val) :"memory"
    );
}

#define atomic_xor atomic_xor
static __always_inline void atomic_xor(atomic_t *atomic, atomic_t val)
{
    asm volatile (
        LOCK_PREFIX "xorl %1,%0"
        :"+m" (atomic)
        :"ir"(val) :"memory"
    );
}

// #define atomic_xchg atomic_xchg
// static __always_inline int atomic_xchg(atomic_t *v, int new)
// {
//     return xchg(&v->counter, new);
// }

// #define atomic_cmpxchg atomic_cmpxchg
// static __always_inline int atomic_cmpxchg(atomic_t *v, int old, int new)
// {
//     return cmpxchg(&v->counter, old, new);
// }

// #define atomic_fetch_add atomic_fetch_add
// static __always_inline int atomic_fetch_add(int i, atomic_t *v)
// {
//     return xadd(&v->counter, i);
// }

// #define atomic_fetch_sub atomic_fetch_sub
// static __always_inline int atomic_fetch_sub(int i, atomic_t *v)
// {
//     return xadd(&v->counter, -i);
// }

// #define atomic_fetch_and atomic_fetch_and
// static __always_inline int atomic_fetch_and(int i, atomic_t *v)
// {
//     int val = atomic_read(v);

//     do { } while (!atomic_try_cmpxchg(v, &val, val & i));

//     return val;
// }

// #define atomic_fetch_or atomic_fetch_or
// static __always_inline int atomic_fetch_or(int i, atomic_t *v)
// {
//     int val = atomic_read(v);

//     do { } while (!atomic_try_cmpxchg(v, &val, val | i));

//     return val;
// }

// #define atomic_fetch_xor atomic_fetch_xor
// static __always_inline int atomic_fetch_xor(int i, atomic_t *v)
// {
//     int val = atomic_read(v);

//     do { } while (!atomic_try_cmpxchg(v, &val, val ^ i));

//     return val;
// }

/**
 * atomic_sub_and_test - subtract value from variable and test result
 * @i: integer value to subtract
 * @v: pointer of type atomic_t
 *
 * Atomically subtracts @i from @v and returns
 * true if the result is zero, or false for all
 * other cases.
 */
#define atomic_sub_test atomic_sub_test
static __always_inline bool atomic_sub_test(atomic_t *atomic, atomic_t nr)
{
    return GEN_BINARY_RMWcc(LOCK_PREFIX "subl", atomic, e, "er", nr);
}

/**
 * atomic_inc_and_test - increment and test
 * @v: pointer of type atomic_t
 *
 * Atomically increments @v by 1
 * and returns true if the result is zero, or false for all
 * other cases.
 */
#define atomic_inc_test atomic_inc_test
static __always_inline bool atomic_inc_test(atomic_t *atomic)
{
    return GEN_UNARY_RMWcc(LOCK_PREFIX "incl", atomic, e);
}

/**
 * atomic_dec_and_test - decrement and test
 * @v: pointer of type atomic_t
 *
 * Atomically decrements @v by 1 and
 * returns true if the result is zero, or false for all other
 * cases.
 */
#define atomic_dec_test atomic_dec_test
static __always_inline bool atomic_dec_test(atomic_t *atomic)
{
    return GEN_UNARY_RMWcc(LOCK_PREFIX "decl", atomic, e);
}

// /**
//  * atomic_add_negative - add and test if negative
//  * @i: integer value to add
//  * @v: pointer of type atomic_t
//  *
//  * Atomically adds @i to @v and returns true
//  * if the result is negative, or false when
//  * result is greater than or equal to zero.
//  */
// #define atomic_add_negative atomic_add_negative
// static __always_inline bool atomic_add_negative(int i, atomic_t *atomic)
// {
//     return GEN_BINARY_RMWcc(LOCK_PREFIX "addl", atomic, s, "er", i);
// }

// /**
//  * atomic_add_return - add integer and return
//  * @i: integer value to add
//  * @v: pointer of type atomic_t
//  *
//  * Atomically adds @i to @v and returns @i + @v
//  */
// #define atomic_add_return atomic_add_return
// static __always_inline int atomic_add_return(int i, atomic_t *v)
// {
//     return i + xadd(&v->counter, i);
// }

// /**
//  * atomic_sub_return - subtract integer and return
//  * @v: pointer of type atomic_t
//  * @i: integer value to subtract
//  *
//  * Atomically subtracts @i from @v and returns @v - @i
//  */
// #define atomic_sub_return atomic_sub_return
// static __always_inline int atomic_sub_return(int i, atomic_t *v)
// {
//     return atomic_add_return(-i, v);
// }

// #define atomic_try_cmpxchg atomic_try_cmpxchg
// static __always_inline bool atomic_try_cmpxchg(atomic_t *v, int *old, int new)
// {
//     return try_cmpxchg(&v->counter, old, new);
// }

#endif /* _ASM_X86_ATOMIC_H */
