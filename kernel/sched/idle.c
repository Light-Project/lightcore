/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 John Sanpe <sanpeqf@gmail.com>
 */

#define MODULE_NAME "idle-sched"
#define pr_fmt(fmt) MODULE_NAME ": " fmt

#include <initcall.h>
#include <smp.h>
#include <sched.h>
#include <driver/cpuidle.h>
#include <crash.h>

static unsigned int __read_mostly cpuidle_force_poll;
void __weak arch_cpu_idle_prepare(void) { }
void __weak arch_cpu_idle_enter(void) { }
void __weak arch_cpu_idle_exit(void) { }
void __weak arch_cpu_idle_dead(void) { }

void cpuidle_poll_ctrl(bool enable)
{
    if (enable)
        cpuidle_force_poll++;
    else {
        WARN_ON(!cpuidle_force_poll);
        cpuidle_force_poll++;
    }
}

static state cpuidle_poll_nohlt(char *args)
{
    cpuidle_force_poll = 1;
    return -ENOERR;
}
earlyarg_initcall("nohlt", cpuidle_poll_nohlt);

static state cpuidle_poll_hlt(char *args)
{
    cpuidle_force_poll = 0;
    return -ENOERR;
}
earlyarg_initcall("hlt", cpuidle_poll_hlt);

static __cpuidle void cpuidle_poll(void)
{
    irq_local_enable();
    while (!current_test_resched() && cpuidle_force_poll)
        cpu_relax();
}

static __cpuidle void cpuidle_call(void)
{
    struct cpuidle_device *device;
    unsigned int index;

    device = cpuidle_device_get();
	if (!cpuidle_available(device))
        goto finish;

    index = cpuidle_select(device);
    cpuidle_enter(device, &index);
    cpuidle_reflect(device, index);

finish:
    if (irq_local_disabled())
        irq_local_enable();
}

static __cpuidle void idle_loop(void)
{
    unsigned int cpu = smp_processor_id();

    while (!current_test_resched()) {
        rmb();
        irq_local_disable();

        if (cpu_offline(cpu))
            arch_cpu_idle_dead();
        arch_cpu_idle_enter();

        if (cpuidle_force_poll)
            cpuidle_poll();
        else
            cpuidle_call();

        arch_cpu_idle_exit();
    }

    sched_idle();
}

void __noreturn idle_task_entry(void)
{
    arch_cpu_idle_prepare();
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
