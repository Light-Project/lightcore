/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2022 Sanpe <sanpeqf@gmail.com>
 */

#include <semaphore.h>
#include <sched.h>
#include <export.h>

struct semaphore_waiter {
    struct list_head list;
    struct sched_task *task;
    bool up;
};

static state common_down(struct semaphore *sem, unsigned long state, ttime_t timeout)
{
    struct semaphore_waiter waiter;

    waiter.task = current;
    waiter.up = false;
    list_add_prev(&sem->waits, &waiter.list);

    for (;;) {
        current_set_state(state);
        if (unlikely(timeout <= 0))
            goto error_time;
        spin_unlock_irq(&sem->lock);
        timeout = sched_timeout(timeout);
        spin_lock_irq(&sem->lock);
        if (waiter.up)
            break;
    }

    return -ENOERR;

error_time:
    list_del(&waiter.list);
    return -ETIME;
}

state semaphore_down(struct semaphore *sem)
{
    irqflags_t irqflags;
    state retval;

    spin_lock_irqsave(&sem->lock, &irqflags);
    if (likely(!sem->count))
        retval = common_down(sem, SCHED_TASK_UNINTERRUPTIBLE, TTIME_MAX);
    else {
        sem->count--;
        retval = -ENOERR;
    }
    spin_unlock_irqrestore(&sem->lock, &irqflags);

    return retval;
}
EXPORT_SYMBOL(semaphore_down);

state semaphore_down_timeout(struct semaphore *sem, ttime_t timeout)
{
    irqflags_t irqflags;
    state retval;

    spin_lock_irqsave(&sem->lock, &irqflags);
    if (unlikely(!sem->count))
        retval = common_down(sem, SCHED_TASK_UNINTERRUPTIBLE, timeout);
    else {
        sem->count--;
        retval = -ENOERR;
    }
    spin_unlock_irqrestore(&sem->lock, &irqflags);

    return retval;
}
EXPORT_SYMBOL(semaphore_down_timeout);

bool semaphore_down_trylock(struct semaphore *sem)
{
    irqflags_t irqflags;
	int count;

    spin_lock_irqsave(&sem->lock, &irqflags);
	count = sem->count - 1;
    if (count >= 0)
        sem->count = count;
    spin_unlock_irqrestore(&sem->lock, &irqflags);

	return count >= 0;
}
EXPORT_SYMBOL(semaphore_down_trylock);

void semaphore_up(struct semaphore *sem)
{
    struct semaphore_waiter *waiter;
    irqflags_t irqflags;

    spin_lock_irqsave(&sem->lock, &irqflags);
    if (likely(list_check_empty(&sem->waits)))
        sem->count++;
    else {
        waiter = list_first_entry(&sem->waits, struct semaphore_waiter, list);
        waiter->up = true;
        list_del(&waiter->list);
        sched_wake_up(waiter->task);
    }
    spin_unlock_irqrestore(&sem->lock, &irqflags);
}
EXPORT_SYMBOL(semaphore_up);
