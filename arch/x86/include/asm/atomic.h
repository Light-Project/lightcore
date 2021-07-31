#ifndef _ASM_ATOMIC_H
#define _ASM_ATOMIC_H

#include <atomic.h>
#include <asm/asm.h>
#include <asm/alternative.h>
#include <asm/rmwcc.h>

/*
 * Atomic operations that C can't guarantee us.  Useful for
 * resource counting etc..
 */

/**
 * atomic_read - read atomic variable
 * @v: pointer of type atomic_t
 *
 * Atomically reads the value of @v.
 */
static __always_inline int atomic_read(const atomic_t *v)
{
	/*
	 * Note for KASAN: we deliberately don't use READ_ONCE_NOCHECK() here,
	 * it's non-inlined function that increases binary size and stack usage.
	 */
	return __READ_ONCE((v)->counter);
}

/**
 * atomic_set - set atomic variable
 * @v: pointer of type atomic_t
 * @i: required value
 *
 * Atomically sets the value of @v to @i.
 */
static __always_inline void atomic_set(atomic_t *v, int i)
{
	__WRITE_ONCE(v->counter, i);
}

/**
 * atomic_add - add integer to atomic variable
 * @i: integer value to add
 * @v: pointer of type atomic_t
 *
 * Atomically adds @i to @v.
 */
static __always_inline void atomic_add(int i, atomic_t *v)
{
	asm volatile(LOCK_PREFIX "addl %1,%0"
		     : "+m" (v->counter)
		     : "ir" (i) : "memory");
}

/**
 * atomic_sub - subtract integer from atomic variable
 * @i: integer value to subtract
 * @v: pointer of type atomic_t
 *
 * Atomically subtracts @i from @v.
 */
static __always_inline void atomic_sub(int i, atomic_t *v)
{
	asm volatile(LOCK_PREFIX "subl %1,%0"
		     : "+m" (v->counter)
		     : "ir" (i) : "memory");
}

/**
 * atomic_sub_and_test - subtract value from variable and test result
 * @i: integer value to subtract
 * @v: pointer of type atomic_t
 *
 * Atomically subtracts @i from @v and returns
 * true if the result is zero, or false for all
 * other cases.
 */
#define atomic_sub_and_test atomic_sub_and_test
static __always_inline bool atomic_sub_and_test(int i, atomic_t *v)
{
	return GEN_BINARY_RMWcc(LOCK_PREFIX "subl", v->counter, e, "er", i);
}

/**
 * atomic_inc - increment atomic variable
 * @v: pointer of type atomic_t
 *
 * Atomically increments @v by 1.
 */
#define atomic_inc atomic_inc
static __always_inline void atomic_inc(atomic_t *v)
{
	asm volatile(LOCK_PREFIX "incl %0"
		     : "+m" (v->counter) :: "memory");
}

/**
 * atomic_dec - decrement atomic variable
 * @v: pointer of type atomic_t
 *
 * Atomically decrements @v by 1.
 */
#define atomic_dec atomic_dec
static __always_inline void atomic_dec(atomic_t *v)
{
	asm volatile(LOCK_PREFIX "decl %0"
		     : "+m" (v->counter) :: "memory");
}

/**
 * atomic_dec_and_test - decrement and test
 * @v: pointer of type atomic_t
 *
 * Atomically decrements @v by 1 and
 * returns true if the result is 0, or false for all other
 * cases.
 */
#define atomic_dec_and_test atomic_dec_and_test
static __always_inline bool atomic_dec_and_test(atomic_t *v)
{
	return GEN_UNARY_RMWcc(LOCK_PREFIX "decl", v->counter, e);
}

/**
 * atomic_inc_and_test - increment and test
 * @v: pointer of type atomic_t
 *
 * Atomically increments @v by 1
 * and returns true if the result is zero, or false for all
 * other cases.
 */
#define atomic_inc_and_test atomic_inc_and_test
static __always_inline bool atomic_inc_and_test(atomic_t *v)
{
	return GEN_UNARY_RMWcc(LOCK_PREFIX "incl", v->counter, e);
}

/**
 * atomic_add_negative - add and test if negative
 * @i: integer value to add
 * @v: pointer of type atomic_t
 *
 * Atomically adds @i to @v and returns true
 * if the result is negative, or false when
 * result is greater than or equal to zero.
 */
#define atomic_add_negative atomic_add_negative
static __always_inline bool atomic_add_negative(int i, atomic_t *v)
{
	return GEN_BINARY_RMWcc(LOCK_PREFIX "addl", v->counter, s, "er", i);
}

/**
 * atomic_add_return - add integer and return
 * @i: integer value to add
 * @v: pointer of type atomic_t
 *
 * Atomically adds @i to @v and returns @i + @v
 */
#define atomic_add_return atomic_add_return
static __always_inline int atomic_add_return(int i, atomic_t *v)
{
	return i + xadd(&v->counter, i);
}

/**
 * atomic_sub_return - subtract integer and return
 * @v: pointer of type atomic_t
 * @i: integer value to subtract
 *
 * Atomically subtracts @i from @v and returns @v - @i
 */
#define atomic_sub_return atomic_sub_return
static __always_inline int atomic_sub_return(int i, atomic_t *v)
{
	return atomic_add_return(-i, v);
}

#define atomic_fetch_add atomic_fetch_add
static __always_inline int atomic_fetch_add(int i, atomic_t *v)
{
	return xadd(&v->counter, i);
}

#define atomic_fetch_sub atomic_fetch_sub
static __always_inline int atomic_fetch_sub(int i, atomic_t *v)
{
	return xadd(&v->counter, -i);
}

#define atomic_cmpxchg atomic_cmpxchg
static __always_inline int atomic_cmpxchg(atomic_t *v, int old, int new)
{
	return cmpxchg(&v->counter, old, new);
}

#define atomic_try_cmpxchg atomic_try_cmpxchg
static __always_inline bool atomic_try_cmpxchg(atomic_t *v, int *old, int new)
{
	return try_cmpxchg(&v->counter, old, new);
}

#define atomic_xchg atomic_xchg
static __always_inline int atomic_xchg(atomic_t *v, int new)
{
	return xchg(&v->counter, new);
}

static __always_inline void atomic_and(int i, atomic_t *v)
{
	asm volatile(LOCK_PREFIX "andl %1,%0"
			: "+m" (v->counter)
			: "ir" (i)
			: "memory");
}

#define atomic_fetch_and atomic_fetch_and
static __always_inline int atomic_fetch_and(int i, atomic_t *v)
{
	int val = atomic_read(v);

	do { } while (!atomic_try_cmpxchg(v, &val, val & i));

	return val;
}

static __always_inline void atomic_or(int i, atomic_t *v)
{
	asm volatile(LOCK_PREFIX "orl %1,%0"
			: "+m" (v->counter)
			: "ir" (i)
			: "memory");
}

#define atomic_fetch_or atomic_fetch_or
static __always_inline int atomic_fetch_or(int i, atomic_t *v)
{
	int val = atomic_read(v);

	do { } while (!atomic_try_cmpxchg(v, &val, val | i));

	return val;
}

static __always_inline void atomic_xor(int i, atomic_t *v)
{
	asm volatile(LOCK_PREFIX "xorl %1,%0"
			: "+m" (v->counter)
			: "ir" (i)
			: "memory");
}

#define atomic_fetch_xor atomic_fetch_xor
static __always_inline int atomic_fetch_xor(int i, atomic_t *v)
{
	int val = atomic_read(v);

	do { } while (!atomic_try_cmpxchg(v, &val, val ^ i));

	return val;
}
 
#endif /* _ASM_ATOMIC_H */