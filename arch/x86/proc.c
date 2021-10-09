/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <kernel.h>
#include <asm/idt.h>
#include <asm/irq.h>
#include <asm/proc.h>

state arch_switch_task(struct task *prev, struct task *next)
{
    return -ENOERR;
}

void __noreturn cpu_halt(void)
{
    cpu_irq_disable();
    for (;;)
    asm volatile("rep; hlt");
}

void __noreturn cpu_reset(void)
{
    cpu_irq_disable();
    idt_int_gate(PF_FAULT, NULL);
    idt_int_gate(GP_FAULT, NULL);
    idt_int_gate(DF_ABORT, NULL);
    for (;;) /* Trigger triple fault */
    asm volatile("int $14");
}