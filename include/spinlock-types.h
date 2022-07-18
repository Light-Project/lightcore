/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _SPINLOCK_TYPES_H_
#define _SPINLOCK_TYPES_H_

#include <spinlock-raw-types.h>

typedef struct spinlock {
    raw_spinlock_t rlock;
} spinlock_t;

typedef struct rwlock {
    raw_rwlock_t rlock;
} rwlock_t;

#define SPIN_LOCK_STATIC(name) \
    {{}}

#define SPIN_LOCK_UNLOCK(name) \
    (spinlock_t) SPIN_LOCK_STATIC(name)

#define SPIN_LOCK(name) \
    spinlock_t name = SPIN_LOCK_UNLOCK(name)

#define SPIN_LOCK(name) \
    spinlock_t name = SPIN_LOCK_UNLOCK(name)

static inline void spin_lock_init(spinlock_t *spinlock)
{
    *spinlock = SPIN_LOCK_UNLOCK(spinlock);
}

#endif  /* _SPINLOCK_TYPES_H_ */
