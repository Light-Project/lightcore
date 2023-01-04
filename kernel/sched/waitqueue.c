/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2022 Sanpe <sanpeqf@gmail.com>
 */

#include <waitqueue.h>
#include <sched.h>
#include <export.h>

void waitqueue_wait_prepare_locked(struct wait_queue_head *queue, struct wait_queue *node)
{
    node->task = current;
    if (list_check_empty(&node->list))
        list_add_prev(&queue->waits, &node->list);
}
EXPORT_SYMBOL(waitqueue_wait_prepare_locked);

void waitqueue_wait_state_locked(struct wait_queue_head *queue, struct wait_queue *node, unsigned long state)
{
    current_set_state(state);
    waitqueue_wait_prepare_locked(queue, node);
}
EXPORT_SYMBOL(waitqueue_wait_state_locked);

void waitqueue_wait_prepare(struct wait_queue_head *queue, struct wait_queue *node)
{
    irqflags_t irqflags;

    spin_lock_irqsave(&queue->lock, &irqflags);
    waitqueue_wait_prepare_locked(queue, node);
    spin_unlock_irqrestore(&queue->lock, &irqflags);
}
EXPORT_SYMBOL(waitqueue_wait_prepare);

void waitqueue_wait_state(struct wait_queue_head *queue, struct wait_queue *node, unsigned long state)
{
    irqflags_t irqflags;

    spin_lock_irqsave(&queue->lock, &irqflags);
    waitqueue_wait_state_locked(queue, node, state);
    spin_unlock_irqrestore(&queue->lock, &irqflags);
}
EXPORT_SYMBOL(waitqueue_wait_state);

void waitqueue_wakeup_one_locked(struct wait_queue_head *queue)
{
	struct wait_queue *first;

    if (list_check_empty(&queue->waits))
        return;

    first = list_first_entry(&queue->waits, struct wait_queue, list);
    list_del(&first->list);
    sched_wake_up(first->task);
}
EXPORT_SYMBOL(waitqueue_wakeup_one_locked);

void waitqueue_wakeup_all_locked(struct wait_queue_head *queue)
{
    while (!list_check_empty(&queue->waits))
        waitqueue_wakeup_one_locked(queue);
}
EXPORT_SYMBOL(waitqueue_wakeup_all_locked);

void waitqueue_wakeup_one(struct wait_queue_head *queue)
{
    irqflags_t irqflags;

    spin_lock_irqsave(&queue->lock, &irqflags);
    waitqueue_wakeup_one_locked(queue);
    spin_unlock_irqrestore(&queue->lock, &irqflags);
}
EXPORT_SYMBOL(waitqueue_wakeup_one);

void waitqueue_wakeup_all(struct wait_queue_head *queue)
{
    irqflags_t irqflags;

    spin_lock_irqsave(&queue->lock, &irqflags);
    waitqueue_wakeup_all_locked(queue);
    spin_unlock_irqrestore(&queue->lock, &irqflags);
}
EXPORT_SYMBOL(waitqueue_wakeup_all);
