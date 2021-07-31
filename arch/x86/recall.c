/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com> 
 */

#include <printk.h>

#include <asm/recall.h>
#include <asm/regs.h>

void show_regs(struct regs *regs)
{
    printk("EIP: %08lx EFLAGS: %08lx\n", regs->eip, regs->eflags);
    printk("EAX: %08lx EBX: %08lx ECX: %08lx EDX: %08lx\n",
            regs->eax, regs->ebx, regs->ecx, regs->edx);
    printk("ESI: %08lx EDI: %08lx EBP: %08lx ESP: %08lx\n",
            regs->esi, regs->edi, regs->ebp, regs->esp);
    printk("CS: %04x DS: %04x ES: %04x FS: %04x GS: %04x SS: %04x\n",
            regs->cs, regs->ds, regs->es, regs->fs, regs->gs, regs->ss);
    printk("CR0: %08x CR2: %08x CR3: %08x CR4: %08x\n", 
            cr0_get(), cr2_get(), cr3_get(), cr4_get());
}

void show_stack(void *sp)
{

}

void recall(const char *str, struct regs *regs)
{
    show_regs(regs);
}
