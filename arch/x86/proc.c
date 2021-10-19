/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <kernel.h>
#include <export.h>

#include <asm/regs.h>
#include <asm/gdt.h>
#include <asm/idt.h>
#include <asm/irq.h>
#include <asm/proc.h>

state proc_thread_switch(struct task *prev, struct task *next)
{
    return -ENOERR;
}

void proc_thread_setup(struct regs *regs, size_t ip, size_t sp)
{
    regs->eip    = ip;
    regs->esp    = sp;
    regs->cs     = GDT_ENTRY_KERNEL_CS_BASE;
    regs->ds     = GDT_ENTRY_KERNEL_DS_BASE;
    regs->es     = GDT_ENTRY_KERNEL_DS_BASE;
    regs->ss     = GDT_ENTRY_KERNEL_DS_BASE;
    regs->fs     = 0;
    regs->eflags = EFLAGS_IF;
}

void __noreturn proc_halt(void)
{
    cpu_irq_disable();
    for (;;)
    asm volatile("rep; hlt");
}

void __noreturn proc_reset(void)
{
    cpu_irq_disable();
    idt_int_gate(PF_FAULT, NULL);
    idt_int_gate(GP_FAULT, NULL);
    idt_int_gate(DF_ABORT, NULL);
    for (;;) /* Trigger triple fault */
    asm volatile("int $14");
}

EXPORT_SYMBOL(proc_halt);
EXPORT_SYMBOL(proc_reset);
