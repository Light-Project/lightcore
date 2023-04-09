/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 John Sanpe <sanpeqf@gmail.com>
 */

#include <mutex.h>
#include <sched.h>
#include <export.h>

struct mutex_waiter {
    struct list_head list;
    struct sched_task *task;
};

/**
 * @WAITERS: non-empty waiter list; unlock must issue a wakeup.
 * @HANDOFF: unlock needs to hand the lock to the top-waiter.
 * @PICKUP: handoff has been done and we're waiting for pickup.
 */
#define MUTEX_FLAG_WAITERS  BIT(0)
#define MUTEX_FLAG_HANDOFF  BIT(1)
#define MUTEX_FLAG_PICKUP   BIT(2)
#define MUTEX_FLAGS         BIT_RANGE(2, 0)

static inline unsigned long owner_flags(unsigned long owner)
{
    return owner & MUTEX_FLAGS;
}

static inline struct sched_task *owner_task(unsigned long owner)
{
    return (void *)(owner & ~MUTEX_FLAGS);
}

static inline unsigned long owner_make(struct sched_task *task, unsigned long flags)
{
    return (unsigned long)task | flags;
}

static inline void mutex_flag_set(struct mutex *mutex, unsigned long flags)
{
    atomic_or(&mutex->owner, flags);
}

static inline void mutex_flag_clear(struct mutex *mutex, unsigned long flags)
{
    atomic_and(&mutex->owner, ~flags);
}

/**
 * mutex_fast_lock() - try to use mutex lock fast channel.
 * @mutex: the mutex to be fast lock.
 *
 * Optimistic try-lock that only works in the uncontended case.
 * Make sure to follow with an other solution before failing.
 *
 * @return: true on successfully locked or false on failure.
 */
static inline bool mutex_fast_lock(struct mutex *mutex)
{
    unsigned long curr = (uintptr_t)current;
    unsigned long zero = 0;
    return atomic_try_cmpxchg(&mutex->owner, &zero, curr);
}

/**
 * mutex_fast_unlock() - try to use mutex unlock fast channel.
 * @mutex: the mutex to be fast unlock.
 *
 * Optimistic try-unlock that only works in the monopoly case.
 * Make sure to follow with an other solution before failing.
 *
 * @return: true on successfully unlocked or false on failure.
 */
static inline bool mutex_fast_unlock(struct mutex *mutex)
{
    unsigned long curr = (uintptr_t)current;
    return atomic_try_cmpxchg(&mutex->owner, &curr, 0UL);
}

/**
 * mutex_waiter_enqueue()
 * @mutex: the mutex to be enqueue.
 * @waiter: waiter pointer.
 *
 * Add @waiter to a given wait_list in the mutex lock and set the
 * FLAG_WAITERS flag if it's the first waiter.
 *
 * @return: true if it's the first waiter.
 */
static bool mutex_waiter_enqueue(struct mutex *mutex, struct mutex_waiter *waiter)
{
    list_add_prev(&mutex->waits, &waiter->list);
    if (list_check_first(&mutex->waits, &waiter->list)) {
        mutex_flag_set(mutex, MUTEX_FLAG_WAITERS);
        return true;
    }
    return false;
}

/**
 * mutex_waiter_dequeue()
 * @mutex: the mutex to be dequeue.
 * @waiter: waiter pointer.
 *
 * Delete @waiter form a given wait_list in the mutex lock and set the
 * FLAG_WAITERS flag if it's the last waiter.
 */
static void mutex_waiter_dequeue(struct mutex *mutex, struct mutex_waiter *waiter)
{
    list_del(&waiter->list);
    if (list_check_empty(&mutex->waits))
        mutex_flag_clear(mutex, MUTEX_FLAG_WAITERS);
}

static bool mutex_waiter_first(struct mutex *mutex, struct mutex_waiter *waiter)
{
    return list_first_entry(&mutex->waits, struct mutex_waiter, list) == waiter;
}

/**
 * mutex_handoff()
 * @mutex: the mutex to be handoff.
 * @task: next task pointer.
 */
static void mutex_handoff(struct mutex *mutex, struct sched_task *task)
{
    unsigned long owner, new;

    owner = atomic_read(&mutex->owner);
    for (;;) {
        new = (owner & MUTEX_FLAG_WAITERS);
        new |= (unsigned long)task;
        if (task)
            new |= MUTEX_FLAG_PICKUP;

        if (atomic_try_cmpxchg(&mutex->owner, &owner, new))
            break;
    }
}

static inline struct sched_task *mutex_common_trylock(struct mutex *mutex, bool handoff)
{
    struct sched_task *curr = current;
    unsigned long owner;

    owner = atomic_read(&mutex->owner);
    for (;;) {
        struct sched_task *task;
        unsigned long flags;

        task = owner_task(owner);
        flags = owner_flags(owner);

        if (!task)
            task = curr;
        else {
            if (flags & MUTEX_FLAG_PICKUP) {
                if (task != curr)
                    break;
                flags &= ~MUTEX_FLAG_PICKUP;
            } else if (handoff) {
                if (flags & MUTEX_FLAG_HANDOFF)
                    break;
                flags |= MUTEX_FLAG_HANDOFF;
            } else
                break;
        }

        if (atomic_try_cmpxchg(&mutex->owner, &owner, owner_make(task, flags))) {
            if (task == curr) /* success */
                return NULL;
            break;
        }
    }

    return owner_task(owner);
}

static __sched void mutex_common_lock(struct mutex *mutex, unsigned long state)
{
    struct mutex_waiter waiter;
    bool first;

    preempt_disable();
    if (mutex_common_trylock(mutex, false))
        goto exit_preempt;

    spin_lock(&mutex->lock);
    if (mutex_common_trylock(mutex, false))
        goto exit_unlock;

    waiter.task = current;
    mutex_waiter_enqueue(mutex, &waiter);

    current_set_state(state);
    for (;;) {
        if (mutex_common_trylock(mutex, false))
            goto finish;

        spin_unlock(&mutex->lock);
        sched_resched();
        preempt_disable();

        first = mutex_waiter_first(mutex, &waiter);
        current_set_state(state);

        if (!mutex_common_trylock(mutex, first))
            break;

        spin_lock(&mutex->lock);
    }

    spin_lock(&mutex->lock);
finish:

    current_set_state(SCHED_TASK_RUNNING);
    mutex_waiter_dequeue(mutex, &waiter);

exit_unlock:
    spin_unlock(&mutex->lock);
exit_preempt:
    preempt_enable();
}

static __sched void mutex_slowpath_lock(struct mutex *mutex)
{
    mutex_common_lock(mutex, SCHED_TASK_UNINTERRUPTIBLE);
}

static __sched void mutex_slowpath_unlock(struct mutex *mutex)
{
    struct sched_task *wake = NULL;
    DEFINE_WAKEQUEUE(queue);
    unsigned long owner;

    owner = atomic_read(&mutex->owner);
    for (;;) {
        if (owner & MUTEX_FLAG_HANDOFF)
            break;

        if (atomic_try_cmpxchg(&mutex->owner, &owner, owner_flags(owner))) {
            if (owner & MUTEX_FLAG_WAITERS)
                break;
            return;
        }
    }

    spin_lock(&mutex->lock);
    if (!list_check_empty(&mutex->waits)) {
        struct mutex_waiter *waiter;
        waiter = list_first_entry(&mutex->waits, struct mutex_waiter, list);
        wake = waiter->task;
        wakequeue_add(&queue, wake);
    }

    if (owner & MUTEX_FLAG_HANDOFF)
        mutex_handoff(mutex, wake);

    spin_unlock(&mutex->lock);
    wakequeue_action(&queue);
}

bool __sched mutex_trylock(struct mutex *mutex)
{
    return !mutex_common_trylock(mutex, false);
}
EXPORT_SYMBOL(mutex_trylock);

/**
 * mutex_lock() - acquire the mutex.
 * @mutex: the mutex to be acquired.
 *
 * Lock the mutex exclusively for this task. If the mutex is not
 * available right now, it will sleep until it can get it.
 */
void __sched mutex_lock(struct mutex *mutex)
{
    if (likely(mutex_fast_lock(mutex)))
        return;
    mutex_slowpath_lock(mutex);
}
EXPORT_SYMBOL(mutex_lock);

/**
 * mutex_unlock() - release the mutex.
 * @mutex: the mutex to be release.
 *
 * Unlock a mutex that has been locked by this task previously.
 */
void __sched mutex_unlock(struct mutex *mutex)
{
    if (likely(mutex_fast_unlock(mutex)))
        return;
    mutex_slowpath_unlock(mutex);
}
EXPORT_SYMBOL(mutex_unlock);
