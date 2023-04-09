/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2022 John Sanpe <sanpeqf@gmail.com>
 */

#include <wakequeue.h>
#include <sched.h>
#include <export.h>

static bool common_add(struct wake_queue_head *queue, struct sched_task *task)
{
    struct wake_queue *node = &task->wakeq;

    if (cmpxchg(&node->next, NULL, WAKEQUEUE_TAIL))
        return false;

    *queue->tailp = node;
    queue->tailp = &node->next;

    return true;
}

/**
 * wakequeue_add - queue a wakeup for 'later' waking.
 * @head: the wake_queue_head to add @task to.
 * @task: the task to queue for 'later' wakeup.
 */
void wakequeue_add(struct wake_queue_head *queue, struct sched_task *task)
{
    if (common_add(queue, task))
        sched_task_get(task);
}
EXPORT_SYMBOL(wakequeue_add);

/**
 * wake_q_add_safe - safely queue a wakeup for 'later' waking.
 * @queue: the wake_queue_head to add @task to.
 * @task: the task to queue for 'later' wakeup.
 */
void wakequeue_add_safe(struct wake_queue_head *queue, struct sched_task *task)
{
    if (!common_add(queue, task))
        sched_task_put(task);
}
EXPORT_SYMBOL(wakequeue_add_safe);

/**
 * wakequeue_action - wake up all blocked tasks on the queue.
 * @queue: queue to wake.
 */
void wakequeue_action(struct wake_queue_head *queue)
{
    struct wake_queue *node = queue->first;
    struct sched_task *task;

    while (node != WAKEQUEUE_TAIL) {
        task = container_of(node, struct sched_task, wakeq);
        node = node->next;
        task->wakeq.next = NULL;

        sched_wake_up(task);
        sched_task_put(task);
    }
}
EXPORT_SYMBOL(wakequeue_action);
