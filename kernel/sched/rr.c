/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#define MODULE_NAME "rr-sched"
#define pr_fmt(fmt) MODULE_NAME ": " fmt

#include <bitmap.h>
#include <kmalloc.h>
#include <initcall.h>
#include <sched.h>
#include <printk.h>

struct kcache *rr_cache;
#define RR_PRIO_MAX 64

struct rr_task {
    struct task task;
    struct list_head list;
};

#define task_to_rr(tsk) \
    container_of(tsk, struct rr_task, task)

struct rr_queue {
    DEFINE_BITMAP(bitmap, RR_PRIO_MAX);
    struct list_head task[RR_PRIO_MAX];
    struct sched_queue *sched;
};

static __always_inline struct rr_queue *
queue_get_rr(struct sched_queue *queue)
{
    return queue->priv[SCHED_PRIO_RR];
}

static __always_inline void
queue_set_rr(struct sched_queue *queue, struct rr_queue *rr)
{
    queue->priv[SCHED_PRIO_RR] = rr;
}

static state rr_queue_create(struct sched_queue *queue)
{
    struct rr_queue *rr_queue;

    rr_queue = kzalloc(sizeof(*rr_queue), GFP_KERNEL);
    if (!rr_queue)
        return -ENOMEM;

    queue->priv[SCHED_PRIO_RR] = rr_queue;
    return -ENOERR;
}

static void rr_queue_destroy(struct sched_queue *queue)
{
    struct rr_queue *rr_queue = queue_get_rr(queue);

    kfree(rr_queue);
}

static struct task *rr_task_create(int flags)
{
    struct rr_task *rr_task;

    rr_task = kcache_zalloc(rr_cache, GFP_KERNEL);
    if (!rr_task)
        return NULL;

    return &rr_task->task;
}

static void rr_task_destroy(struct task *task)
{
    struct rr_task *rr_task = task_to_rr(task);

    list_del(&rr_task->list);
    kcache_free(rr_cache, rr_task);
}

static void rr_task_enqueue(struct sched_queue *queue, struct task *task, int flags)
{

}

static void rr_task_dequeue(struct sched_queue *queue, struct task *task, int flags)
{

}

static void rr_task_tick(struct sched_queue *rueue, struct task *task)
{

}

static void rr_task_next(struct sched_queue *rueue, struct task *task)
{

}

static void rr_task_prev(struct sched_queue *rueue, struct task *task)
{

}

static struct task *rr_task_pick(struct sched_queue *queue)
{
    struct rr_queue *rr_queue = queue_get_rr(queue);
    struct rr_task *rr_task;
    struct list_head *prio;
    unsigned int index;

    index = bitmap_find_first(rr_queue->bitmap, RR_PRIO_MAX);
    if (index >= RR_PRIO_MAX)
        return NULL;

    prio = rr_queue->task + index;
    rr_task = list_first_entry(prio, struct rr_task, list);

    return &rr_task->task;
}

static struct sched_type rr_sched = {
    .name = MODULE_NAME,
    .priority = SCHED_PRIO_RR,

    .queue_create  = rr_queue_create,
    .queue_destroy = rr_queue_destroy,

    .task_create  = rr_task_create,
    .task_destroy = rr_task_destroy,

    .task_enqueue = rr_task_enqueue,
    .task_dequeue = rr_task_dequeue,

    .task_tick = rr_task_tick,
    .task_next = rr_task_next,
    .task_prev = rr_task_prev,
    .task_pick = rr_task_pick,
};

static state rr_sched_init(void)
{
#ifdef CONFIG_SCHED_DEF_RR
    default_sched = &rr_sched;
#endif
    rr_cache = kcache_create(MODULE_NAME, sizeof(struct rr_task), KCACHE_PANIC);
    return sched_register(&rr_sched);
}
scheduler_initcall(rr_sched_init);
