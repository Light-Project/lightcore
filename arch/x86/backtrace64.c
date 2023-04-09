/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 John Sanpe <sanpeqf@gmail.com>
 */

#include <printk.h>
#include <panic.h>
#include <asm/backtrace.h>
#include <asm/traps.h>

void show_regs(struct regs *regs, unsigned long error_code)
{
    uint32_t cr0, cr2, cr3, cr4;
    uint64_t dr0, dr1, dr2, dr3, dr6, dr7;

    printk("registers:\n");
    printk(" RIP: 0x%016llx EFLAGS: 0x%08llx ERROR: %08lx\n",
            regs->ip, regs->flags, error_code);
    printk(" RAX: 0x%016llx RBX: 0x%016llx RCX: 0x%016llx\n",
            regs->ax, regs->bx, regs->cx);
    printk(" RDX: 0x%016llx RSI: 0x%016llx RDI: 0x%016llx\n",
            regs->dx, regs->si, regs->di);
    printk(" RBP: 0x%016llx RSP: 0x%016llx R08: 0x%016llx\n",
            regs->bp, regs->sp, regs->r8);
    printk(" R09: 0x%016llx R10: 0x%016llx R11: 0x%016llx\n",
            regs->r9, regs->r10, regs->r11);
    printk(" R12: 0x%016llx R13: 0x%016llx R14: 0x%016llx\n",
            regs->r12, regs->r13, regs->r14);

    printk(" CS: 0x%04llx DS: 0x%04llx SS: 0x%04llx\n", regs->cs, regs->ds, regs->ss);
    printk(" ES: 0x%04llx FS: 0x%04llx GS: 0x%04llx\n", regs->es, regs->fs, regs->gs);

    cr0 = cr0_get();
    cr2 = cr2_get();
    cr3 = cr3_get();
    cr4 = cr4_get();

    printk(" CR0: 0x%08x CR2: 0x%08x CR3: 0x%08x CR4: 0x%08x\n", cr0, cr2, cr3, cr4);

    dr0 = dr0_get();
    dr1 = dr1_get();
    dr2 = dr2_get();
    dr3 = dr3_get();
    dr6 = dr6_get();
    dr7 = dr7_get();

    if (dr0 == DR0_DEFAULT && dr1 == DR1_DEFAULT && dr2 == DR2_DEFAULT &&
        dr3 == DR3_DEFAULT && dr6 == DR6_DEFAULT && dr7 == DR7_DEFAULT)
        return;

    printk(" DB0: 0x%08llx DB1: 0x%08llx DB2: 0x%08llx\n", dr0, dr1, dr2);
    printk(" DB3: 0x%08llx DB6: 0x%08llx DB7: 0x%08llx\n", dr3, dr6, dr7);
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

void oops(struct regs *regs, unsigned long error_code, const char *str)
{
    printk("Kernel Oops: %s\n", str);

    show_regs(regs, error_code);
    dump_stack((size_t *)regs->sp);

    panic("fatal exception");
}
