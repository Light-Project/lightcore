/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <initcall.h>
#include <sched.h>

static void fifo_task_setup(struct task *task)
{

}

static void fifo_task_enqueue(struct sched_queue *queue, struct task *task)
{

}

static void fifo_task_dequeue(struct sched_queue *queue, struct task *task)
{

}

static struct sched_type fifo_sched = {
    .name = "fifo",
    .task_setup = fifo_task_setup,
    .task_enqueue = fifo_task_enqueue,
    .task_dequeue = fifo_task_dequeue,
};

static state fifo_init(void)
{
    return sched_register(&fifo_sched);
}
sched_initcall(fifo_init);