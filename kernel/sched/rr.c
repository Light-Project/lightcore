/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <initcall.h>
#include <sched.h>

struct rr_task {
    struct task task;
};

struct rr_sched_queue {
    struct sched_queue queue;
};

static void rr_task_enqueue(struct sched_queue *queue, struct task *task)
{

}

static void rr_task_dequeue(struct sched_queue *queue, struct task *task)
{

}

static struct sched_type rr_sched = {
    .name = "rr",
    .priority =  -10,
    // .task_create  = rr_task_create,
    .task_enqueue = rr_task_enqueue,
    .task_dequeue = rr_task_dequeue,
    // .task_tick    = rr_task_tick,
};

static state rr_sched_init(void)
{
    return sched_register(&rr_sched);
}
scheduler_initcall(rr_sched_init);
