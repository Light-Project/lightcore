/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#define MODULE_NAME "fifo-sched"
#define pr_fmt(fmt) MODULE_NAME ": " fmt

#include <initcall.h>
#include <sched.h>

struct kcache *fifo_cache;

struct rr_queue {
    DEFINE_BITMAP(bitmap, RR_PRIO_MAX);
    struct list_head task[RR_PRIO_MAX];
    struct sched_queue *sched;
    unsigned int tasknr;
};

struct fifo_task {
    struct sched_task task;
    struct list_head list;
    unsigned int timeslice;
};

#define task_to_rr(tsk) \
    container_of(tsk, struct rr_task, task)

static void fifo_task_setup(struct sched_task *task)
{

}

static void fifo_task_enqueue(struct sched_queue *queue, struct sched_task *task)
{

}

static void fifo_task_dequeue(struct sched_queue *queue, struct sched_task *task)
{

}

static struct sched_type fifo_sched = {
    .name = MODULE_NAME,

    .queue_create  = fifo_queue_create,
    .queue_destroy = fifo_queue_destroy,

    .task_create  = fifo_task_create,
    .task_destroy = fifo_task_destroy,
    .task_enqueue = fifo_task_enqueue,
    .task_dequeue = fifo_task_dequeue,
    .task_next = fifo_task_next,
    .task_prev = fifo_task_prev,
    .task_yield = fifo_task_yield,
    .task_pick = fifo_task_pick,
};

static state fifo_sched_init(void)
{
#ifdef CONFIG_SCHED_DEF_RR
    default_sched = &fifo_sched;
#endif
    rr_cache = kcache_create(MODULE_NAME, sizeof(struct rr_task), KCACHE_PANIC);
    return sched_register(&fifo_sched);
}
scheduler_initcall(fifo_sched_init);