/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <printk.h>
#include <panic.h>
#include <asm/backtrace.h>
#include <asm/traps.h>

void show_regs(struct regs *regs)
{
	size_t cr0, cr2, cr3, cr4;
	size_t dr0, dr1, dr2, dr3, dr6, dr7;

    printk("Regs:\n");
    printk(" EIP: %08x EFLAGS: %08x ERROR: %08x\n",
            regs->ip, regs->flags, regs->error);
    printk(" EAX: %08x EBX: %08x ECX: %08x EDX: %08x\n",
            regs->ax, regs->bx, regs->cx, regs->dx);
    printk(" ESI: %08x EDI: %08x EBP: %08x ESP: %08x\n",
            regs->si, regs->di, regs->bp, regs->sp);
    printk(" CS: %04x DS: %04x SS: %04x ES: %04x FS: %04x GS: %04x\n",
            regs->cs, regs->ds, regs->ss, regs->es, regs->fs, regs->gs);

    cr0 = cr0_get();
    cr2 = cr2_get();
    cr3 = cr3_get();
    cr4 = cr4_get();

    printk(" CR0: %08lx CR2: %08lx CR3: %08lx CR4: %08lx\n", cr0, cr2, cr3, cr4);

    dr0 = dr0_get();
    dr1 = dr1_get();
    dr2 = dr2_get();
    dr3 = dr3_get();
    dr6 = dr6_get();
    dr7 = dr7_get();

    if (dr0 == DR0_DEFAULT && dr1 == DR1_DEFAULT && dr2 == DR2_DEFAULT &&
        dr3 == DR3_DEFAULT && dr6 == DR6_DEFAULT && dr7 == DR7_DEFAULT)
        return;

    printk(" DB0: %08lx DB1: %08lx DB2: %08lx\n", dr0, dr1, dr2);
    printk(" DB3: %08lx DB6: %08lx DB7: %08lx\n", dr3, dr6, dr7);
}

void dump_stack(size_t *sp)
{
    printk("Stack:\n");
    for (int i = 0; i < 8; i++) {
        printk(" [%p]: ", sp);
        for (int i = 0; i < 4; i++)
            printk("%08lx ", *sp++);
        printk("\n");
    }
}

void backtrace(size_t *bp)
{
    printk("Backtrace:\n");
    while (bp) {
        printk(" [0x%08lx]:", *(bp + 1));
        printk(" (0x%08lx 0x%08lx)\n", *(bp + 2), *(bp + 3));
        bp = (size_t *)*bp;
    }
}

void oops(struct regs *regs, const char *str)
{
    printk("Oops %s\n", str);

    show_regs(regs);
    dump_stack((size_t *)regs->sp);
    backtrace((size_t *)regs->bp);

    panic("fatal exception");
}
