/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _SPINLOCK_TYPES_H_
#define _SPINLOCK_TYPES_H_

#include <spinlock-raw-types.h>

typedef struct {
    raw_spinlock_t rlock;
} spinlock_t;

typedef struct {
    raw_rwlock_t rlock;
} rwlock_t;

#define SPIN_LOCK_STATIC(name) \
    {{}}

#define RW_LOCK_STATIC(name) \
    {{}}

#define SPIN_LOCK_UNLOCK(name) \
    (spinlock_t) SPIN_LOCK_STATIC(name)

#define RW_LOCK_UNLOCK(name) \
    (rwlock_t) RW_LOCK_STATIC(name)

#define SPIN_LOCK(name) \
    spinlock_t name = SPIN_LOCK_UNLOCK(name)

#define RW_LOCK(name) \
    rwlock_t name = RW_LOCK_UNLOCK(name)

static inline void spin_lock_init(spinlock_t *spinlock)
{
    *spinlock = SPIN_LOCK_UNLOCK(spinlock);
}

static inline void rw_lock_init(rwlock_t *rwlock)
{
    *rwlock = RW_LOCK_UNLOCK(rwlock);
}

#endif  /* _SPINLOCK_TYPES_H_ */
