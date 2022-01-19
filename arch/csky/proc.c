/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <asm/proc.h>
#include <asm/irq.h>
#include <export.h>

state proc_thread_switch(struct sched_task *prev, struct sched_task *next)
{
    return -ENOERR;
}

void proc_idle(void)
{
#if defined(CONFIG_CPU_PM_WAIT)
    asm volatile ("wait");
#elif defined(CONFIG_CPU_PM_DOZE)
    asm volatile ("doze");
#elif defined(CONFIG_CPU_PM_STOP)
    asm volatile ("stop");
#endif
}

void __noreturn proc_halt(void)
{
    cpu_irq_disable();
    for (;;)
    proc_idle();
}
EXPORT_SYMBOL(proc_halt);

void __noreturn proc_poweroff(void)
{
    proc_halt();
}
EXPORT_SYMBOL(proc_poweroff);

void __noreturn proc_reset(void)
{
    proc_halt();
}
EXPORT_SYMBOL(proc_reset);
