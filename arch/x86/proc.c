/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <kernel.h>
#include <task.h>
#include <asm/proc.h>
#include <asm/entry.h>
#include <asm/idt.h>
#include <asm/irq.h>

void proc_thread_setup(struct regs *regs, size_t ip, size_t sp)
{
    regs->ip    = ip;
    regs->sp    = sp;
    regs->cs    = GDT_ENTRY_KERNEL_CS_BASE;
    regs->ds    = GDT_ENTRY_KERNEL_DS_BASE;
    regs->es    = GDT_ENTRY_KERNEL_DS_BASE;
    regs->ss    = GDT_ENTRY_KERNEL_DS_BASE;
    regs->flags = EFLAGS_IF;
}

state proc_thread_copy(enum clone_flags flags, struct sched_task *curr, struct sched_task *child, void *arg)
{
    struct proc_inactive_frame *inactive;
    struct proc_frame *frame;
    struct regs *childregs;

    childregs   = stack_regs(child->stack);
    frame       = container_of(childregs, struct proc_frame, regs);
    inactive    = &frame->frame;

    inactive->ret       = (size_t) entry_thread_return;
    inactive->bp        = (size_t) 0;
    child->proc.sp      = (size_t) frame;
    child->proc.flags   = EFLAGS_FIXED;

    return -ENOERR;
}

state proc_thread_switch(struct sched_task *prev, struct sched_task *next)
{
    struct proc_context *pproc = &prev->proc;
    struct proc_context *nproc = &next->proc;

    entry_switch_stack(&prev->stack, next->stack);

    pproc->gs = gs_get();
    if (pproc->gs | nproc->gs)
        gs_set(nproc->gs);

    return -ENOERR;
}

void __noreturn proc_halt(void)
{
    cpu_irq_disable();
    for (;;)
    asm volatile ("rep \n hlt");
}

void __noreturn proc_reset(void)
{
    cpu_irq_disable();
    idt_int_gate(TRAP_PF, NULL);
    idt_int_gate(TRAP_GP, NULL);
    idt_int_gate(TRAP_DF, NULL);
    for (;;) /* Trigger triple fault */
    asm volatile("int $14");
}
