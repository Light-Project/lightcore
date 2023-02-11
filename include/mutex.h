/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _MUTEX_H_
#define _MUTEX_H_

#include <types.h>
#include <spinlock.h>
#include <list.h>

struct mutex {
    atomic_t owner;
    spinlock_t lock;
    struct list_head waits;
};

#define MUTEX_LOCK(name) \
    struct mutex name = {}

extern bool mutex_trylock(struct mutex *mutex);
extern void mutex_lock(struct mutex *mutex);
extern void mutex_unlock(struct mutex *mutex);

#endif  /* _MUTEX_H_ */
