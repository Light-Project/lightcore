/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <printk.h>
#include <asm/backtrace.h>
#include <asm/regs.h>

void show_regs(struct regs *regs)
{
    printk("Regs:\n");
    printk(" EIP: %08x EFLAGS: %08x\n", regs->eip, regs->eflags);
    printk(" EAX: %08x EBX: %08x ECX: %08x EDX: %08x\n",
            regs->eax, regs->ebx, regs->ecx, regs->edx);
    printk(" ESI: %08x EDI: %08x EBP: %08x ESP: %08x\n",
            regs->esi, regs->edi, regs->ebp, regs->esp);
    printk(" CS: %04x DS: %04x ES: %04x FS: %04x GS: %04x SS: %04x\n",
            regs->cs, regs->ds, regs->es, regs->fs, regs->gs, regs->ss);
    printk(" CR0: %08x CR2: %08x CR3: %08x CR4: %08x\n",
            cr0_get(), cr2_get(), cr3_get(), cr4_get());
}

void show_stack(size_t *sp)
{
    printk("Stack:\n");
    for (int i = 0; i < 8; i++) {
        printk(" [%p]: ", sp);
        for (int i = 0; i < 4; i++)
            printk("%08x ", *sp++);
        printk("\n");
    }
}

void back_trace(size_t *bp)
{
    printk("Backtrace:\n");
    while (bp) {
        printk(" [0x%x]:",   *(bp + 1));
        printk(" (0x%08x 0x%08x)\n", *(bp + 2), *(bp + 3));
        bp = (size_t *)*bp;
    }
}

void recall(const char *str, struct regs *regs)
{
    show_regs(regs);
    show_stack((size_t *)regs->esp);
    back_trace((size_t *)regs->ebp);
}
