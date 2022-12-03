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
    regs->ip    = ip;
    regs->sp    = sp;
    regs->cs    = GDT_ENTRY_KERNEL_CS_BASE;
    regs->ds    = GDT_ENTRY_KERNEL_DS_BASE;
    regs->es    = GDT_ENTRY_KERNEL_DS_BASE;
    regs->ss    = GDT_ENTRY_KERNEL_DS_BASE;
    regs->flags = EFLAGS_IF;
}

state proc_thread_copy(struct task_clone_args *args, struct sched_task *child)
{
    struct proc_inactive_frame *inactive;
    struct proc_frame *frame;
    struct regs *childregs;

    if (child->flags & SCHED_TASK_KTHREAD) {
        child->kcontext.mcontext.regs[KC_BUF_FLAGS] = EFLAGS_FIXED;
        child->kcontext.mcontext.regs[KC_BUF_SI] = (unsigned long)current;
        child->kcontext.mcontext.regs[KC_BUF_DI] = (unsigned long)child;

        child->kcontext.stack = child->stack;
        child->kcontext.ssize = THREAD_SIZE;
        makecontext(&child->kcontext, (void *)entry_kthread_return, 2, args->entry, args->arg);
        return -ENOERR;
    }

    childregs   = stack_regs(child->stack);
    frame       = container_of(childregs, struct proc_frame, regs);
    inactive    = &frame->frame;

    inactive->ret       = (size_t) entry_fork_return;
    inactive->bp        = (size_t) 0;
    child->proc.sp      = (size_t) frame;
    child->proc.flags   = EFLAGS_FIXED;

    return -ENOERR;
}

state proc_thread_switch(struct sched_task *prev)
{
    struct proc_context *nproc = &current->proc;
    struct proc_context *pproc = &prev->proc;

    pproc->gs = gs_get();
    if (pproc->gs || nproc->gs)
        gs_set(nproc->gs);

    return -ENOERR;
}

void __noreturn proc_halt(void)
{
    irq_local_disable();
    for (;;)
    asm volatile ("rep; hlt");
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
