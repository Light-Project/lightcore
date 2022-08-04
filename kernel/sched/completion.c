/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2022 Sanpe <sanpeqf@gmail.com>
 */

#include <completion.h>
#include <waitqueue.h>
#include <sched.h>
#include <export.h>

static ttime_t commin_wait(struct completion *comp, unsigned long state, ttime_t timeout)
{
    struct wait_queue node;

    if (!comp->counter) {
        do {
            waitqueue_wait_state_locked(&comp->queue, &node, state);
            spin_unlock_irq(&comp->queue.lock);
            timeout = sched_timeout(timeout);
            spin_lock_irq(&comp->queue.lock);
        } while (!comp->counter && timeout);

        if (!comp->counter)
            return timeout;
    }

    if (comp->counter != UINT_MAX)
        comp->counter--;

    return timeout;
}

void completion_wait(struct completion *comp)
{
    spin_lock_irq(&comp->queue.lock);
    commin_wait(comp, SCHED_TASK_UNINTERRUPTIBLE, TTIME_MAX);
    spin_unlock_irq(&comp->queue.lock);
}
EXPORT_SYMBOL(completion_wait);

ttime_t completion_wait_timeout(struct completion *comp, ttime_t timeout)
{
    spin_lock_irq(&comp->queue.lock);
    timeout = commin_wait(comp, SCHED_TASK_UNINTERRUPTIBLE, timeout);
    spin_unlock_irq(&comp->queue.lock);
    return timeout;
}
EXPORT_SYMBOL(completion_wait_timeout);

void complete_one(struct completion *comp)
{
    irqflags_t irqflags;

    spin_lock_irqsave(&comp->queue.lock, &irqflags);
    if (comp->counter != UINT_MAX)
        comp->counter++;
    waitqueue_wakeup_one_locked(&comp->queue);
    spin_unlock_irqrestore(&comp->queue.lock, &irqflags);
}
EXPORT_SYMBOL(complete_one);

void complete_all(struct completion *comp)
{
    irqflags_t irqflags;

    spin_lock_irqsave(&comp->queue.lock, &irqflags);
    comp->counter = UINT_MAX;
    waitqueue_wakeup_all_locked(&comp->queue);
    spin_unlock_irqrestore(&comp->queue.lock, &irqflags);
}
EXPORT_SYMBOL(complete_all);

bool completion_done(struct completion *comp)
{
    irqflags_t flags;

    if (!READ_ONCE(comp->counter))
        return false;

    spin_lock_irqsave(&comp->queue.lock, &flags);
    spin_unlock_irqrestore(&comp->queue.lock, &flags);
    return true;
}
EXPORT_SYMBOL(completion_done);
