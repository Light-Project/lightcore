/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#define MODULE_NAME "idle-sched"
#define pr_fmt(fmt) MODULE_NAME ": " fmt

#include <initcall.h>
#include <sched.h>
#include <smp.h>

void __weak cpu_idle_prepare(void) { }
void __weak cpu_idle_enter(void) { }
void __weak cpu_idle_exit(void) { }
void __weak cpu_idle_dead(void) { }

static void idle_loop(void)
{
    // unsigned int cpu = smp_processor_id();

    // while (!current_test_resched()) {
    //     rmb();
    //     irq_local_disable();

    //     if (cpu_offline(cpu))
    //         cpu_idle_dead();

    //     cpu_idle_enter();
    //     cpu_idle_exit();
    // }

    scheduler_idle();
}

void __noreturn idle_task_entry(void)
{
    cpu_idle_prepare();
    for (;;)
        idle_loop();
    unreachable();
}

static struct sched_task *idle_task_pick(struct sched_queue *queue)
{
    return queue->idle;
}

struct sched_type idle_sched = {
    .name = MODULE_NAME,
    .priority = SCHED_PRIO_IDLE,
    .task_pick = idle_task_pick,
};

static state idle_sched_init(void)
{
    return sched_register(&idle_sched);
}
scheduler_initcall(idle_sched_init);