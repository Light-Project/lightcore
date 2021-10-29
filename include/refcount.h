/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _REFCOUNT_H_
#define _REFCOUNT_H_

#include <types.h>
#include <asm/atomic.h>

typedef struct refcount {
    atomic_t count;
} refcount_t;

static inline atomic_t refcount_get(struct refcount *ref)
{
    return atomic_read(&ref->count);
}

static inline void refcount_set(struct refcount *ref, atomic_t nr)
{
    atomic_write(&ref->count, nr);
}

static inline atomic_t refcount_add(struct refcount *ref, atomic_t nr)
{
    atomic_t old = refcount_get(ref);



    return old;
}

static inline bool refcount_add_test(struct refcount *ref, atomic_t nr)
{

}

static inline atomic_t refcount_sub(struct refcount *ref, atomic_t nr)
{

}

static inline bool refcount_sub_test(struct refcount *ref, atomic_t nr)
{

}

static inline atomic_t refcount_inc(struct refcount *ref)
{

}

static inline bool refcount_inc_test(struct refcount *ref, atomic_t nr)
{
    return atomic_inc_test(&ref->count);
}

static inline atomic_t refcount_dec(struct refcount *ref)
{

}

static inline bool refcount_dec_test(struct refcount *ref, atomic_t nr)
{
    return atomic_dec_test(&ref->count);
}

#endif  /* _REFCOUNT_H_ */
