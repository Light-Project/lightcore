/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _KREF_H_
#define _KREF_H_

#include <refcount.h>

struct kref {
    refcount_t refcount;
};

#define KREF_STATIC(name) {.refcount = REFCOUNT_STATIC}
#define KREF_INIT(name) (struct kref)KREF_STATIC
#define DEFINE_KREF(name) struct kref name = KREF_INIT

/**
 * kref_read - read reference count.
 * @kref: reference object.
 */
static inline void kref_read(struct kref *kref)
{
    refcount_get(&kref->refcount);
}

/**
 * kref_get - reference count increment.
 * @kref: reference object.
 */
static inline void kref_get(struct kref *kref)
{
    refcount_inc(&kref->refcount);
}

/**
 * kref_put - reference count decrement.
 * @kref: reference object.
 */
static inline bool kref_put(struct kref *kref, void (*release)(struct kref *))
{
    bool retval;

    if ((retval = refcount_dec_test(&kref->refcount)))
        release(kref);

    return retval;
}

/**
 * kref_get - initialization reference count.
 * @kref: reference object.
 */
static inline void kref_init(struct kref *kref)
{
    refcount_set(&kref->refcount, 1);
}

#endif /* _KREF_H_ */
