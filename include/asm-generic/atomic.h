/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _ASM_GENERIC_ATOMIC_H_
#define _ASM_GENERIC_ATOMIC_H_

#include <types.h>
#include <asm/irq.h>
#include <asm/rwonce.h>
#include <asm/cmpxchg.h>

#ifndef atomic_read
static __always_inline atomic_t atomic_read(atomic_t *atomic)
{
    return READ_ONCE(*atomic);
}
#endif

#ifndef atomic_write
static __always_inline void atomic_write(atomic_t *atomic, atomic_t val)
{
    WRITE_ONCE(*atomic, val);
}
#endif

#ifndef CONFIG_SMP

#define ATOMIC_OP(op, sym)                                  \
static __always_inline void                                 \
generic_atomic_##op(atomic_t *atomic, atomic_t val)         \
{                                                           \
    irqflags_t flags;                                       \
                                                            \
    flags = cpu_irq_save();                                 \
    *atomic sym val;                                        \
    cpu_irq_restore(flags);                                 \
}

#define ATOMIC_FETCH_OP(op, sym)                            \
static __always_inline atomic_t                             \
generic_atomic_fetch_##op(atomic_t *atomic, atomic_t val)   \
{                                                           \
    irqflags_t flags;                                       \
    atomic_t old;                                           \
                                                            \
    flags = cpu_irq_save();                                 \
    old = *atomic;                                          \
    *atomic sym val;                                        \
    cpu_irq_restore(flags);                                 \
                                                            \
    return old;                                             \
}

#define ATOMIC_TEST_OP(op, sym)                             \
static __always_inline bool                                 \
generic_atomic_test_##op(atomic_t *atomic, atomic_t val)    \
{                                                           \
    irqflags_t flags;                                       \
    atomic_t ret;                                           \
                                                            \
    flags = cpu_irq_save();                                 \
    *atomic sym val;                                        \
    ret = *atomic;                                          \
    cpu_irq_restore(flags);                                 \
                                                            \
    return !!ret;                                           \
}

#endif  /* CONFIG_SMP */

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

ATOMIC_TEST_OP(add, +=)
ATOMIC_TEST_OP(sub, -=)
ATOMIC_TEST_OP(and, &=)
ATOMIC_TEST_OP(or,  |=)
ATOMIC_TEST_OP(xor, ^=)

#ifndef atomic_add
# define atomic_add         generic_atomic_add
#endif
#ifndef atomic_sub
# define atomic_sub         generic_atomic_sub
#endif
#ifndef atomic_and
# define atomic_and         generic_atomic_and
#endif
#ifndef atomic_or
# define atomic_or          generic_atomic_or
#endif
#ifndef atomic_xor
# define atomic_xor         generic_atomic_xor
#endif

#ifndef atomic_fetch_add
# define atomic_fetch_add   generic_atomic_fetch_add
#endif
#ifndef atomic_fetch_sub
# define atomic_fetch_sub   generic_atomic_fetch_sub
#endif
#ifndef atomic_fetch_and
# define atomic_fetch_and   generic_atomic_fetch_and
#endif
#ifndef atomic_fetch_or
# define atomic_fetch_or    generic_atomic_fetch_or
#endif
#ifndef atomic_fetch_xor
# define atomic_fetch_xor   generic_atomic_fetch_xor
#endif

#ifndef atomic_add_test
# define atomic_add_test    generic_atomic_test_add
#endif
#ifndef atomic_sub_test
# define atomic_sub_test    generic_atomic_test_sub
#endif
#ifndef atomic_and_test
# define atomic_and_test    generic_atomic_test_and
#endif
#ifndef atomic_or_test
# define atomic_or_test     generic_atomic_test_or
#endif
#ifndef atomic_xor_test
# define atomic_xor_test    generic_atomic_test_xor
#endif

#ifndef atomic_inc
static __always_inline void atomic_inc(atomic_t *atomic)
{
    atomic_add(atomic, 1);
}
#endif

#ifndef atomic_dec
static __always_inline void atomic_dec(atomic_t *atomic)
{
    atomic_sub(atomic, 1);
}
#endif

#ifndef atomic_inc_test
static __always_inline bool atomic_inc_test(atomic_t *atomic)
{
    return atomic_add_test(atomic, 1);
}
#endif

#ifndef atomic_dec_test
static __always_inline bool atomic_dec_test(atomic_t *atomic)
{
    return atomic_sub_test(atomic, 1);
}
#endif

#define atomic_xchg(atomic, new) xchg(atomic, new)
#define atomic_cmpxchg(atomic, old, new) cmpxchg(atomic, old, new)

#ifndef atomic_try_cmpxchg
static __always_inline bool atomic_try_cmpxchg(atomic_t *atomic, atomic_t *old, atomic_t new)
{
	atomic_t ret, __old = *old;

	ret = atomic_cmpxchg(atomic, __old, new);
	if (unlikely(ret != __old))
		*old = ret;

	return likely(ret == __old);
}
#endif

#endif  /* _ASM_GENERIC_ATOMIC_H_ */