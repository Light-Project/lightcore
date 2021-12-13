/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <kernel.h>
#include <export.h>
#include <asm/regs.h>
#include <asm/segment.h>
#include <asm/idt.h>
#include <asm/irq.h>
#include <asm/proc.h>

state proc_thread_switch(struct task *prev, struct task *next)
{
    return -ENOERR;
}

void proc_thread_setup(struct regs *regs, size_t ip, size_t sp)
{
    regs->ip    = ip;
    regs->sp    = sp;
    regs->flags = EFLAGS_IF;
    regs->cs    = GDT_ENTRY_KERNEL_CS_BASE;
    regs->ds    = GDT_ENTRY_KERNEL_DS_BASE;
    regs->es    = GDT_ENTRY_KERNEL_DS_BASE;
    regs->ss    = GDT_ENTRY_KERNEL_DS_BASE;
    regs->fs    = 0;
}

void __noreturn proc_halt(void)
{
    cpu_irq_disable();
    for (;;)
    asm volatile("rep; hlt");
}
EXPORT_SYMBOL(proc_halt);

void __noreturn proc_poweroff(void)
{
    proc_halt();
}
EXPORT_SYMBOL(proc_poweroff);

void __noreturn proc_reset(void)
{
    cpu_irq_disable();
    idt_int_gate(TRAP_PF, NULL);
    idt_int_gate(TRAP_GP, NULL);
    idt_int_gate(TRAP_DF, NULL);
    for (;;) /* Trigger triple fault */
    asm volatile("int $14");
}
EXPORT_SYMBOL(proc_reset);
