/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <initcall.h>
#include <sched.h>

static void rr_task_setup(struct task *task)
{

}

static void rr_task_enqueue(struct sched_queue *queue, struct task *task)
{

}

static void rr_task_dequeue(struct sched_queue *queue, struct task *task)
{

}

static struct sched_type rr_sched = {
    .name = "rr",
    .task_setup = rr_task_setup,
    .task_enqueue = rr_task_enqueue,
    .task_dequeue = rr_task_dequeue,
};

static state rr_sched_init(void)
{
    return sched_register(&rr_sched);
}
sched_initcall(rr_sched_init);
