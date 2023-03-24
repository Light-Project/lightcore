/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _WAKEQUEUE_H_
#define _WAKEQUEUE_H_

#include <types.h>
#include <stddef.h>

struct sched_task;

struct wake_queue_head {
    struct wake_queue *first;
    struct wake_queue **tailp;
};

struct wake_queue {
    struct wake_queue *next;
};

#define WAKEQUEUE_TAIL ((struct wake_queue *)0x01)

#define WAKEQUEUE_STATIC(name) {    \
    .first = WAKEQUEUE_TAIL,        \
    .tailp = &(name).first,         \
}

#define WAKEQUEUE_INIT(name) \
    (struct wake_queue_head) WAKEQUEUE_STATIC(name)

#define DEFINE_WAKEQUEUE(name) \
    struct wake_queue_head name = WAKEQUEUE_INIT(name)

static inline void wakequeue_init(struct wake_queue_head *queue)
{
    *queue = WAKEQUEUE_INIT(*queue);
}

static inline bool wakequeue_check_empty(struct wake_queue_head *queue)
{
    return queue->first == WAKEQUEUE_TAIL;
}

extern void wakequeue_add(struct wake_queue_head *queue, struct sched_task *task);
extern void wakequeue_add_safe(struct wake_queue_head *queue, struct sched_task *task);
extern void wakequeue_action(struct wake_queue_head *queue);

#endif /* _WAKEQUEUE_H_ */
