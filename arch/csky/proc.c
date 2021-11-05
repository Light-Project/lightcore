/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <asm/proc.h>
#include <asm/irq.h>
#include <export.h>

state proc_thread_switch(struct task *prev, struct task *next)
{
    return -ENOERR;
}

void __noreturn proc_halt(void)
{
    cpu_irq_disable();
    for (;;)
    asm volatile ("wait");
}

void __noreturn proc_reset(void)
{
    proc_halt();
}

EXPORT_SYMBOL(proc_halt);
EXPORT_SYMBOL(proc_reset);