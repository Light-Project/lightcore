#ifndef _MUTEX_H_
#define _MUTEX_H_

#include <types.h>
#include <system/spinlock.h>
#include <atomic.h>
#include <list.h>

typedef struct mutex{
    atomic_t    atomic;
    list_t      list;
    spinlock_t  lock;
} mutex_t;


void mutex_lock(struct mutex *mutex);
void mutex_unlock(struct mutex *mutex);

#endif
