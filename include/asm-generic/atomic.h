/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _ASM_GENERIC_ATOMIC_H_
#define _ASM_GENERIC_ATOMIC_H_

#include <types.h>
#include <irqflags.h>
#include <asm/cmpxchg.h>

/**
 * arch_atomic_read - read atomic variable
 * @atomic: pointer of type atomic_t
 */
#ifndef arch_atomic_read
static __always_inline atomic_t arch_atomic_read(atomic_t *atomic)
{
    return READ_ONCE(*atomic);
}
#endif

/**
 * arch_atomic_write - write atomic variable
 * @atomic: pointer of type atomic_t
 * @val: required value
 */
#ifndef arch_atomic_write
static __always_inline void arch_atomic_write(atomic_t *atomic, atomic_t val)
{
    WRITE_ONCE(*atomic, val);
}
#endif

#ifdef CONFIG_SMP

#else /* !CONFIG_SMP */

#define ATOMIC_OP(op, sym)                                  \
static __always_inline void                                 \
generic_atomic_##op(atomic_t *atomic, atomic_t val)         \
{                                                           \
    irqflags_t flags;                                       \
                                                            \
    flags = irq_local_save();                               \
    *atomic sym val;                                        \
    irq_local_restore(flags);                               \
}

#define ATOMIC_FETCH_OP(op, sym)                            \
static __always_inline atomic_t                             \
generic_atomic_fetch_##op(atomic_t *atomic, atomic_t val)   \
{                                                           \
    irqflags_t flags;                                       \
    atomic_t old;                                           \
                                                            \
    flags = irq_local_save();                               \
    old = *atomic;                                          \
    *atomic sym val;                                        \
    irq_local_restore(flags);                               \
                                                            \
    return old;                                             \
}

#define ATOMIC_RETURN_OP(op, sym)                           \
static __always_inline atomic_t                             \
generic_atomic_return_##op(atomic_t *atomic, atomic_t val)  \
{                                                           \
    irqflags_t flags;                                       \
    atomic_t ret;                                           \
                                                            \
    flags = irq_local_save();                               \
    *atomic sym val;                                        \
    ret = *atomic;                                          \
    irq_local_restore(flags);                               \
                                                            \
    return ret;                                             \
}

#endif /* CONFIG_SMP */

ATOMIC_OP(add, +=)
ATOMIC_OP(sub, -=)
ATOMIC_OP(and, &=)
ATOMIC_OP(or,  |=)
ATOMIC_OP(xor, ^=)

ATOMIC_FETCH_OP(add, +=)
ATOMIC_FETCH_OP(sub, -=)
ATOMIC_FETCH_OP(and, &=)
ATOMIC_FETCH_OP(or,  |=)
ATOMIC_FETCH_OP(xor, ^=)

ATOMIC_RETURN_OP(add, +=)
ATOMIC_RETURN_OP(sub, -=)
ATOMIC_RETURN_OP(and, &=)
ATOMIC_RETURN_OP(or,  |=)
ATOMIC_RETURN_OP(xor, ^=)

/**
 * arch_atomic_add - add integer to atomic variable
 * @atomic: pointer of type atomic_t
 * @val: required value
 */
#ifndef arch_atomic_add
# define arch_atomic_add(atomic, val)           generic_atomic_add(atomic, val)
#endif

/**
 * arch_atomic_sub - subtract integer from atomic variable
 * @atomic: pointer of type atomic_t
 * @val: required value
 */
#ifndef arch_atomic_sub
# define arch_atomic_sub(atomic, val)           generic_atomic_sub(atomic, val)
#endif

/**
 * arch_atomic_and - and atomic variable
 * @atomic: pointer of type atomic_t
 * @val: required value
 */
#ifndef arch_atomic_and
# define arch_atomic_and(atomic, val)           generic_atomic_and(atomic, val)
#endif

/**
 * arch_atomic_or - or atomic variable
 * @atomic: pointer of type atomic_t
 * @val: required value
 */
#ifndef arch_atomic_or
# define arch_atomic_or(atomic, val)            generic_atomic_or(atomic, val)
#endif

/**
 * arch_atomic_xor - xor atomic variable
 * @atomic: pointer of type atomic_t
 * @val: required value
 */
#ifndef arch_atomic_xor
# define arch_atomic_xor(atomic, val)           generic_atomic_xor(atomic, val)
#endif

/**
 * arch_atomic_fetch_add - fetch and atomic add
 * @atomic: pointer of type atomic_t
 * @val: required value
 */
#ifndef arch_atomic_fetch_add
# define arch_atomic_fetch_add(atomic, val)     generic_atomic_fetch_add(atomic, val)
#endif

/**
 * arch_atomic_fetch_sub - fetch and atomic sub
 * @atomic: pointer of type atomic_t
 * @val: required value
 */
#ifndef arch_atomic_fetch_sub
# define arch_atomic_fetch_sub(atomic, val)     generic_atomic_fetch_sub(atomic, val)
#endif

/**
 * arch_atomic_fetch_and - fetch and atomic and
 * @atomic: pointer of type atomic_t
 * @val: required value
 */
#ifndef arch_atomic_fetch_and
# define arch_atomic_fetch_and(atomic, val)     generic_atomic_fetch_and(atomic, val)
#endif

/**
 * arch_atomic_fetch_or - fetch and atomic or
 * @atomic: pointer of type atomic_t
 * @val: required value
 */
#ifndef arch_atomic_fetch_or
# define arch_atomic_fetch_or(atomic, val)      generic_atomic_fetch_or(atomic, val)
#endif

/**
 * arch_atomic_fetch_xor - fetch and atomic xor
 * @atomic: pointer of type atomic_t
 * @val: required value
 */
#ifndef arch_atomic_fetch_xor
# define arch_atomic_fetch_xor(atomic, val)     generic_atomic_fetch_xor(atomic, val)
#endif

/**
 * arch_atomic_add_return - add value from variable and return result
 * @atomic: pointer of type atomic_t
 * @val: required value
 */
#ifndef arch_atomic_add_return
# define arch_atomic_add_return(atomic, val)    generic_atomic_return_add(atomic, val)
#endif

/**
 * arch_atomic_sub_return - subtract value from variable and return result
 * @atomic: pointer of type atomic_t
 * @val: required value
 */
#ifndef arch_atomic_sub_return
# define arch_atomic_sub_return(atomic, val)    generic_atomic_return_sub(atomic, val)
#endif

/**
 * arch_atomic_and_return - and value from variable and return result
 * @atomic: pointer of type atomic_t
 * @val: required value
 */
#ifndef arch_atomic_and_return
# define arch_atomic_and_return(atomic, val)    generic_atomic_return_and(atomic, val)
#endif

/**
 * arch_atomic_or_return - or value from variable and return result
 * @atomic: pointer of type atomic_t
 * @val: required value
 */
#ifndef arch_atomic_or_return
# define arch_atomic_or_return(atomic, val)     generic_atomic_return_or(atomic, val)
#endif

/**
 * arch_atomic_xor_return - xor value from variable and return result
 * @atomic: pointer of type atomic_t
 * @val: required value
 */
#ifndef arch_atomic_xor_return
# define arch_atomic_xor_return(atomic, val)    generic_atomic_return_xor(atomic, val)
#endif

/**
 * arch_atomic_add_test - add value from variable and test result
 * @atomic: pointer of type atomic_t
 * @val: required value
 */
#ifndef arch_atomic_add_test
static __always_inline bool arch_atomic_add_test(atomic_t *atomic, atomic_t val)
{
    return !arch_atomic_add_return(atomic, val);
}
#endif

/**
 * arch_atomic_sub_test - subtract value from variable and test result
 * @atomic: pointer of type atomic_t
 * @val: required value
 */
#ifndef arch_atomic_sub_test
static __always_inline bool arch_atomic_sub_test(atomic_t *atomic, atomic_t val)
{
    return !arch_atomic_sub_return(atomic, val);
}
#endif

/**
 * arch_atomic_and_test - and value from variable and test result
 * @atomic: pointer of type atomic_t
 * @val: required value
 */
#ifndef arch_atomic_and_test
static __always_inline bool arch_atomic_and_test(atomic_t *atomic, atomic_t val)
{
    return !arch_atomic_and_return(atomic, val);
}
#endif

/**
 * arch_atomic_or_test - or value from variable and test result
 * @atomic: pointer of type atomic_t
 * @val: required value
 */
#ifndef arch_atomic_or_test
static __always_inline bool arch_atomic_or_test(atomic_t *atomic, atomic_t val)
{
    return !arch_atomic_or_return(atomic, val);
}
#endif

/**
 * arch_atomic_xor_test - xor value from variable and test result
 * @atomic: pointer of type atomic_t
 * @val: required value
 */
#ifndef arch_atomic_xor_test
static __always_inline bool arch_atomic_xor_test(atomic_t *atomic, atomic_t val)
{
    return !arch_atomic_xor_return(atomic, val);
}
#endif

/**
 * arch_atomic_inc - increment atomic variable
 * @atomic: pointer of type atomic_t
 */
#ifndef arch_atomic_inc
static __always_inline void arch_atomic_inc(atomic_t *atomic)
{
    arch_atomic_add(atomic, 1);
}
#endif

/**
 * arch_atomic_dec - decrement atomic variable
 * @atomic: pointer of type atomic_t
 */
#ifndef arch_atomic_dec
static __always_inline void arch_atomic_dec(atomic_t *atomic)
{
    arch_atomic_sub(atomic, 1);
}
#endif

/**
 * arch_atomic_inc_return - increment and return result
 * @atomic: pointer of type atomic_t
 */
#ifndef arch_atomic_fetch_inc
static __always_inline atomic_t arch_atomic_fetch_inc(atomic_t *atomic)
{
    return arch_atomic_fetch_add(atomic, 1);
}
#endif

/**
 * arch_atomic_dec_return - decrement and return result
 * @atomic: pointer of type atomic_t
 */
#ifndef arch_atomic_fetch_dec
static __always_inline atomic_t arch_atomic_fetch_dec(atomic_t *atomic)
{
    return arch_atomic_fetch_sub(atomic, 1);
}
#endif

/**
 * arch_atomic_inc_return - increment and return result
 * @atomic: pointer of type atomic_t
 */
#ifndef arch_atomic_inc_return
static __always_inline atomic_t arch_atomic_inc_return(atomic_t *atomic)
{
    return arch_atomic_add_return(atomic, 1);
}
#endif

/**
 * arch_atomic_dec_return - decrement and return result
 * @atomic: pointer of type atomic_t
 */
#ifndef arch_atomic_dec_return
static __always_inline atomic_t arch_atomic_dec_return(atomic_t *atomic)
{
    return arch_atomic_sub_return(atomic, 1);
}
#endif

/**
 * arch_atomic_inc_test - increment and test result
 * @atomic: pointer of type atomic_t
 */
#ifndef arch_atomic_inc_test
static __always_inline bool arch_atomic_inc_test(atomic_t *atomic)
{
    return !arch_atomic_inc_return(atomic);
}
#endif

/**
 * arch_atomic_dec_test - decrement and test result
 * @atomic: pointer of type atomic_t
 */
#ifndef arch_atomic_dec_test
static __always_inline bool arch_atomic_dec_test(atomic_t *atomic)
{
    return !arch_atomic_dec_return(atomic);
}
#endif

/**
 * atomic_xchg - xchg atomic variable
 * @atomic: pointer of type atomic_t
 * @val: required value
 */
#ifndef arch_atomic_xchg
# define arch_atomic_xchg(atomic, val) arch_xchg(atomic, val)
#endif

/**
 * arch_atomic_cmpxchg - cmp and xchg atomic variable
 * @atomic: pointer of type atomic_t
 * @val: required value
 */
#ifndef arch_atomic_cmpxchg
# define arch_atomic_cmpxchg(atomic, old, val) arch_cmpxchg(atomic, old, val)
#endif

/**
 * arch_atomic_try_cmpxchg - try to cmp and xchg atomic variable
 * @atomic: pointer of type atomic_t
 * @old: required value
 * @val: required value
 */
#ifndef arch_atomic_try_cmpxchg
static __always_inline bool arch_atomic_try_cmpxchg(atomic_t *atomic, atomic_t *old, atomic_t val)
{
    atomic_t ret, __old = *old;

    ret = arch_atomic_cmpxchg(atomic, __old, val);
    if (unlikely(ret != __old))
        *old = ret;

    return likely(ret == __old);
}
#endif

#endif /* _ASM_GENERIC_ATOMIC_H_ */
