/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#define MODULE_NAME "rr-sched"
#define pr_fmt(fmt) MODULE_NAME ": " fmt

#include <sched.h>
#include <bitmap.h>
#include <kmalloc.h>
#include <initcall.h>
#include <printk.h>

#define RR_PRIO_MAX 64
#define RR_TIMESLICE (CONFIG_RR_TIMESLICE * CONFIG_SYSTICK_FREQ / 1000)
static struct kcache *rr_cache;

struct rr_queue {
    DEFINE_BITMAP(bitmap, RR_PRIO_MAX);
    struct list_head task[RR_PRIO_MAX];
    unsigned int running[RR_PRIO_MAX];
    struct sched_queue *sched;
};

struct rr_task {
    struct sched_task task;
    struct list_head list;
    unsigned int timeslice;
};

#define task_to_rr(tsk) \
    container_of(tsk, struct rr_task, task)

static __always_inline struct rr_queue *
queue_get_rr(struct sched_queue *queue)
{
    return queue->pdata[SCHED_PRIO_RR];
}

static __always_inline void
queue_set_rr(struct sched_queue *queue, struct rr_queue *rr)
{
    queue->pdata[SCHED_PRIO_RR] = rr;
}

static __always_inline bool
move_entity(enum sched_queue_flags flags)
{
    return !((flags & (SCHED_QUEUE_SAVE | SCHED_QUEUE_MOVE)) == SCHED_QUEUE_SAVE);
}

static state rr_queue_create(struct sched_queue *queue)
{
    struct rr_queue *rr_queue;

    rr_queue = kzalloc(sizeof(*rr_queue), GFP_KERNEL);
    if (!rr_queue)
        return -ENOMEM;

    list_head_init(rr_queue->task);
    queue_set_rr(queue, rr_queue);

    return -ENOERR;
}

static void rr_queue_destroy(struct sched_queue *queue)
{
    struct rr_queue *rr_queue = queue_get_rr(queue);
    kfree(rr_queue);
}

static void rr_task_enqueue(struct sched_queue *queue, struct sched_task *task, enum sched_queue_flags flags)
{
    struct rr_queue *rr_queue = queue_get_rr(queue);
    struct rr_task *rr_task = task_to_rr(task);
    unsigned int prio = task->priority;

    if (move_entity(flags)) {
        if (flags & SCHED_ENQUEUE_HEAD)
            list_add(&rr_queue->task[prio], &rr_task->list);
        else
            list_add_prev(&rr_queue->task[prio], &rr_task->list);
        bit_set(rr_queue->bitmap, task->priority);
    }

    rr_queue->running[prio]++;
}

static void rr_task_dequeue(struct sched_queue *queue, struct sched_task *task, enum sched_queue_flags flags)
{
    struct rr_queue *rr_queue = queue_get_rr(queue);
    struct rr_task *rr_task = task_to_rr(task);
    unsigned int prio = task->priority;

    if (move_entity(flags)) {
        list_del(&rr_task->list);
        if (list_check_empty(&rr_queue->task[prio]))
            bit_clr(rr_queue->bitmap, task->priority);
    }

    rr_queue->running[prio]--;
}

static struct sched_task *rr_task_create(int numa)
{
    struct rr_task *rr_task;

    rr_task = kcache_alloc_numa(rr_cache, GFP_KERNEL, numa);
    if (!rr_task)
        return NULL;

    rr_task->timeslice = RR_TIMESLICE;
    list_head_init(&rr_task->list);

    return &rr_task->task;
}

static void rr_task_destroy(struct sched_task *task)
{
    struct rr_task *rr_task = task_to_rr(task);
    list_del(&rr_task->list);
    kcache_free(rr_cache, rr_task);
}

static void rr_task_tick(struct sched_queue *queue, struct sched_task *task)
{
    struct rr_queue *rr_queue = queue_get_rr(queue);
    struct rr_task *rr_task = task_to_rr(task);
    unsigned int prio = task->priority;

    if (rr_task->timeslice--)
        return;

    if (rr_queue->running[prio] > 1)
        list_move_prev(&rr_queue->task[prio], &rr_task->list);

    rr_task->timeslice = RR_TIMESLICE;
    queue_set_resched(queue);
}

static void rr_task_next(struct sched_queue *queue, struct sched_task *task)
{

}

static void rr_task_prev(struct sched_queue *queue, struct sched_task *task)
{

}

static void rr_task_yield(struct sched_queue *queue)
{
    struct rr_queue *rr_queue = queue_get_rr(queue);
    struct rr_task *rr_task = task_to_rr(queue->curr);
    unsigned int prio = rr_task->task.priority;

    rr_task->timeslice = RR_TIMESLICE;
    list_move_prev(&rr_queue->task[prio], &rr_task->list);
}

static struct sched_task *rr_task_pick(struct sched_queue *queue)
{
    struct rr_queue *rr_queue = queue_get_rr(queue);
    struct rr_task *rr_task;
    struct list_head *prio;
    unsigned int index;

    index = find_first_bit(rr_queue->bitmap, RR_PRIO_MAX);
    if ((index >= RR_PRIO_MAX))
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
    .task_enqueue = rr_task_enqueue,
    .task_dequeue = rr_task_dequeue,

    .task_create  = rr_task_create,
    .task_destroy = rr_task_destroy,
    .task_tick = rr_task_tick,
    .task_next = rr_task_next,
    .task_prev = rr_task_prev,
    .task_yield = rr_task_yield,
    .task_pick = rr_task_pick,
};

static state rr_sched_init(void)
{
#ifdef CONFIG_SCHED_DEF_RR
    default_sched = &rr_sched;
#endif
    rr_cache = KCACHE_CREATE(struct rr_task, KCACHE_PANIC);
    return sched_register(&rr_sched);
}
scheduler_initcall(rr_sched_init);
