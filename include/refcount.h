/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _REFCOUNT_H_
#define _REFCOUNT_H_

#include <types.h>
#include <stddef.h>
#include <atomic.h>

typedef struct refcount {
    atomic_t count;
} refcount_t;

enum refcount_saturation {
    REFCOUNT_ADDNZ_OVF,
    REFCOUNT_ADDNZ_LEAK,
    REFCOUNT_ADD_UAF,
    REFCOUNT_ADD_OVF,
    REFCOUNT_ADD_LEAK,
    REFCOUNT_SUB_UAF,
    REFCOUNT_SUB_OVF,
    REFCOUNT_SUB_LEAK,
};

#define REFCOUNT_STATIC { 1 }
#define REFCOUNT_INIT (refcount_t)REFCOUNT_STATIC
#define DEFINE_REFCOUNT(name) refcount_t name = REFCOUNT_INIT

#ifdef CONFIG_DEBUG_REFCOUNT
extern void refcount_saturation_report(refcount_t *ref, enum refcount_saturation type);
#endif

static inline atomic_t refcount_get(refcount_t *ref)
{
    return atomic_read(&ref->count);
}

static inline void refcount_set(refcount_t *ref, atomic_t nr)
{
    atomic_write(&ref->count, nr);
}

static inline void refcount_fetch_add(refcount_t *ref, atomic_t nr, atomic_t *oldp)
{
    atomic_t old = atomic_fetch_add(&ref->count, nr);

    if (oldp)
        *oldp = old;

#ifdef CONFIG_DEBUG_REFCOUNT
    if (unlikely(!old))
        refcount_saturation_report(ref, REFCOUNT_ADD_UAF);
    else if (unlikely(old < 0))
        refcount_saturation_report(ref, REFCOUNT_ADD_LEAK);
    else if (unlikely(old + nr < 0))
        refcount_saturation_report(ref, REFCOUNT_ADD_OVF);
#endif
}

static inline bool refcount_fetch_addnz_test(refcount_t *ref, atomic_t nr, atomic_t *oldp)
{
    atomic_t old = refcount_get(ref);

    do {
        if (!old)
            break;
    } while (atomic_try_cmpxchg(&ref->count, &old, old + nr));

    if (oldp)
        *oldp = old;

#ifdef CONFIG_DEBUG_REFCOUNT
    if (unlikely(old < 0))
        refcount_saturation_report(ref, REFCOUNT_SUB_LEAK);
    else if (unlikely(old - nr < 0))
        refcount_saturation_report(ref, REFCOUNT_SUB_OVF);
#endif

    return !old;
}

static inline bool refcount_fetch_sub_test(refcount_t *ref, atomic_t nr, atomic_t *oldp)
{
	atomic_t old = atomic_fetch_sub(&ref->count, nr);

    if (oldp)
        *oldp = old;

    if (old == nr)
        return true;

#ifdef CONFIG_DEBUG_REFCOUNT
    if (unlikely(!old))
        refcount_saturation_report(ref, REFCOUNT_SUB_UAF);
    else if (unlikely(old < 0))
        refcount_saturation_report(ref, REFCOUNT_SUB_LEAK);
    else if (unlikely(old - nr < 0))
        refcount_saturation_report(ref, REFCOUNT_SUB_OVF);
#endif

    return false;
}

#define REFCOUNT_OPS(addsub, incdec, operation)                                     \
static inline void                                                                  \
generic_refcount_##addsub(refcount_t *ref, atomic_t nr)                             \
{                                                                                   \
    operation(ref, nr, NULL);                                                       \
}                                                                                   \
                                                                                    \
static inline void                                                                  \
generic_refcount_##incdec(refcount_t *ref)                                          \
{                                                                                   \
    operation(ref, 1, NULL);                                                        \
}                                                                                   \
                                                                                    \
static inline void                                                                  \
generic_refcount_fetch_##incdec(refcount_t *ref, atomic_t *oldp)                    \
{                                                                                   \
    operation(ref, 1, oldp);                                                        \
}

#define REFCOUNT_TEST_OPS(addsub, incdec, operation)                                \
static inline void                                                                  \
generic_refcount_fetch_##addsub(refcount_t *ref, atomic_t nr, atomic_t *oldp)       \
{                                                                                   \
    operation(ref, nr, oldp);                                                       \
}                                                                                   \
                                                                                    \
static inline bool                                                                  \
generic_refcount_test_##addsub(refcount_t *ref, atomic_t nr)                        \
{                                                                                   \
    return operation(ref, nr, NULL);                                                \
}                                                                                   \
                                                                                    \
static inline bool                                                                  \
generic_refcount_test_##incdec(refcount_t *ref)                                     \
{                                                                                   \
    return operation(ref, 1, NULL);                                                 \
}                                                                                   \
                                                                                    \
static inline bool                                                                  \
generic_refcount_fetch_test_##incdec(refcount_t *ref, atomic_t *oldp)               \
{                                                                                   \
    return operation(ref, 1, oldp);                                                 \
}                                                                                   \
REFCOUNT_OPS(addsub, incdec, operation)

REFCOUNT_OPS(add, inc, refcount_fetch_add)
#define refcount_add(ref, nr)                   generic_refcount_add(ref, nr)
#define refcount_inc(ref)                       generic_refcount_inc(ref)
#define refcount_fetch_inc(ref, oldp)           generic_refcount_fetch_inc(ref, oldp)

REFCOUNT_TEST_OPS(addnz, incnz, refcount_fetch_addnz_test)
#define refcount_addnz(ref, nr)                 generic_refcount_addnz(ref, nr)
#define refcount_addnz_test(ref, nr)            generic_refcount_test_addnz(ref, nr)
#define refcount_fetch_addnz(ref, nr, oldp)     generic_refcount_fetch_addnz(ref, nr, oldp)
#define refcount_incnz(ref)                     generic_refcount_incnz(ref)
#define refcount_incnz_test(ref)                generic_refcount_test_incnz(ref)
#define refcount_fetch_incnz(ref, oldp)         generic_refcount_fetch_incnz(ref, oldp)
#define refcount_fetch_incnz_test(ref, oldp)    generic_refcount_fetch_test_incnz(ref, oldp)

REFCOUNT_TEST_OPS(sub, dec, refcount_fetch_sub_test)
#define refcount_sub(ref, nr)                   generic_refcount_sub(ref, nr)
#define refcount_sub_test(ref, nr)              generic_refcount_test_sub(ref, nr)
#define refcount_fetch_sub(ref, nr, oldp)       generic_refcount_fetch_sub(ref, nr, oldp)
#define refcount_dec(ref)                       generic_refcount_dec(ref)
#define refcount_dec_test(ref)                  generic_refcount_test_dec(ref)
#define refcount_fetch_dec(ref, oldp)           generic_refcount_fetch_dec(ref, oldp)
#define refcount_fetch_dec_test(ref, oldp)      generic_refcount_fetch_test_dec(ref, oldp)

#endif  /* _REFCOUNT_H_ */
