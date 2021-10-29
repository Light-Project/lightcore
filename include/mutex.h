/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _MUTEX_H_
#define _MUTEX_H_

#include <types.h>
#include <spinlock.h>
#include <list.h>

struct mutex{
    atomic_t    atomic;
    list_t      list;
    spinlock_t  lock;
};

#define MUTEX_LOCK(name) \
    struct mutex name

void mutex_lock(struct mutex *mutex);
void mutex_unlock(struct mutex *mutex);

#endif
