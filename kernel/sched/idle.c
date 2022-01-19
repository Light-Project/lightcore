/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#define MODULE_NAME "idle-sched"
#define pr_fmt(fmt) MODULE_NAME ": " fmt

#include <initcall.h>
#include <sched.h>

static void idle_loop(void)
{
    cpu_relax();
}

void __noreturn idle_task_entry(void)
{
    for (;;)
        idle_loop();
    unreachable();
}

static struct sched_task *idle_task_pick(struct sched_queue *queue)
{
    return queue->idle;
}

static struct sched_type idle_sched = {
    .name = MODULE_NAME,
    .priority = SCHED_PRIO_IDLE,
    .task_pick = idle_task_pick,
};

static state idle_sched_init(void)
{
    return sched_register(&idle_sched);
}
scheduler_initcall(idle_sched_init);