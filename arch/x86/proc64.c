/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <kernel.h>
#include <task.h>
#include <proc.h>
#include <kcontext.h>
#include <irqflags.h>
#include <asm/idt.h>

void proc_thread_setup(struct regs *regs, size_t ip, size_t sp)
{

}

state proc_thread_copy(struct task_clone_args *args, struct sched_task *child)
{
    return -ENOERR;
}

state proc_thread_switch(struct sched_task *prev)
{
    return -ENOERR;
}

void __noreturn proc_halt(void)
{
    irq_local_disable();
    for (;;)
    asm volatile ("rep \n hlt");
}

void __noreturn proc_reset(void)
{
    irq_local_disable();
    arch_idt_int_gate(TRAP_PF, NULL);
    arch_idt_int_gate(TRAP_GP, NULL);
    arch_idt_int_gate(TRAP_DF, NULL);
    for (;;) /* Trigger triple fault */
    asm volatile("int $14");
}
