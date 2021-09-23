/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <mm.h>
#include <initcall.h>
#include <sched.h>

struct rr_task {
    struct task task;
};

struct rr_sched_queue {
    struct sched_queue queue;
};

static struct task *rr_task_create(enum task_flags flags)
{
    struct rr_task *rr_task;

    rr_task = kzalloc(sizeof(*rr_task), GFP_KERNEL);

    return NULL;
}

static void rr_task_enqueue(struct sched_queue *queue, struct task *task)
{

}

static void rr_task_dequeue(struct sched_queue *queue, struct task *task)
{

}

static struct sched_type rr_sched = {
    .name = "rr",
    .priority =  -10,
    .task_create  = rr_task_create,
    .task_enqueue = rr_task_enqueue,
    .task_dequeue = rr_task_dequeue,
};

static state rr_sched_init(void)
{
    return sched_register(&rr_sched);
}
scheduler_initcall(rr_sched_init);
