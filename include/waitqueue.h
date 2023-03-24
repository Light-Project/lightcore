/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _WAITQUEUE_H_
#define _WAITQUEUE_H_

#include <spinlock.h>
#include <list.h>

struct sched_task;

struct wait_queue_head {
    spinlock_t lock;
    struct list_head waits;
};

struct wait_queue {
    struct list_head list;
    struct sched_task *task;
};

#define WAITQUEUE_HEAD_STATIC(name) { \
    .waits = LIST_HEAD_STATIC((name).waits), \
    .lock = SPIN_LOCK_STATIC(name), \
}

#define WAITQUEUE_STATIC(name, ctask) { \
    .list = LIST_HEAD_STATIC((name).list), \
    .task = (ctask), \
}

#define WAITQUEUE_HEAD_INIT(name) \
    (struct wait_queue_head) WAITQUEUE_HEAD_STATIC(name)

#define WAITQUEUE_INIT(name, ctask) \
    (struct wait_queue) WAITQUEUE_STATIC(name, ctask)

#define DEFINE_WAITQUEUE_HEAD(name) \
    struct wait_queue_head name = WAITQUEUE_HEAD_INIT(name)

#define DEFINE_WAITQUEUE(name, ctask) \
    struct wait_queue name = WAITQUEUE_INIT(name, ctask)

static inline void waitqueue_head_init(struct wait_queue_head *queue)
{
    *queue = WAITQUEUE_HEAD_INIT(*queue);
}

static inline void waitqueue_init(struct wait_queue *queue, struct sched_task *task)
{
    *queue = WAITQUEUE_INIT(*queue, task);
}

/* Wait queue enqueue */
extern void waitqueue_wait_prepare_locked(struct wait_queue_head *queue, struct wait_queue *node);
extern void waitqueue_wait_state_locked(struct wait_queue_head *queue, struct wait_queue *node, unsigned long state);
extern void waitqueue_wait_prepare(struct wait_queue_head *queue, struct wait_queue *node);
extern void waitqueue_wait_state(struct wait_queue_head *queue, struct wait_queue *node, unsigned long state);

/* Wait queue dequeue */
extern void waitqueue_wakeup_one_locked(struct wait_queue_head *queue);
extern void waitqueue_wakeup_all_locked(struct wait_queue_head *queue);
extern void waitqueue_wakeup_one(struct wait_queue_head *queue);
extern void waitqueue_wakeup_all(struct wait_queue_head *queue);

#endif /* _WAITQUEUE_H_ */
