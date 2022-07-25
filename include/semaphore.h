/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _SEMAPHORE_H_
#define _SEMAPHORE_H_

#include <spinlock.h>
#include <list.h>

struct semaphore {
    spinlock_t lock;
    struct list_head waits;
    unsigned int count;
};

#define SEMAPHORE_STATIC(name, value) {         \
    .waits = LIST_HEAD_STATIC((name).waits),    \
    .lock = SPIN_LOCK_STATIC(name),             \
    .count = (value),                           \
}

#define SEMAPHORE_INIT(name, value) \
    (struct semaphore) SEMAPHORE_STATIC(name, value)

#define DEFINE_SEMAPHORE(name, value) \
    struct semaphore name = SEMAPHORE_INIT(name, value)

static inline void semaphore_init(struct semaphore *sem, unsigned int value)
{
    *sem = SEMAPHORE_INIT(*sem, value);
}

extern state semaphore_down(struct semaphore *sem);
extern state semaphore_down_timeout(struct semaphore *sem, ttime_t timeout);
extern bool semaphore_down_trylock(struct semaphore *sem);
extern void semaphore_up(struct semaphore *sem);

#endif  /* _SEMAPHORE_H_ */
