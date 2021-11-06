/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <kernel.h>
#include <printk.h>
#include <asm/backtrace.h>
#include <asm/page.h>
#include <asm/regs.h>

struct stackframe {
    uint32_t fp;
    uint32_t pc;
};

void backtrace_regs(struct regs *regs)
{
    printk("regs:\n");
    printk(" PC: 0x%08x PSR: 0x%08x\n", regs->pc, regs->psr);
    printk(" A0: 0x%08x A1: 0x%08x A2: 0x%08x A3: 0x%08x\n",
            regs->a0, regs->a1, regs->a2, regs->a3);
    printk(" A4: 0x%08x A5: 0x%08x L0: 0x%08x L1: 0x%08x\n",
            regs->a4, regs->a5, regs->l0, regs->l1);
    printk(" L2: 0x%08x L3: 0x%08x L4: 0x%08x L5: 0x%08x\n",
            regs->l2, regs->l3, regs->l4, regs->l5);
    printk(" R1: 0x%08x SP: 0x%08x LR: 0x%08x GB: 0x%08x\n",
            regs->r1, regs->sp, regs->lr, regs->gb);
}

void backtrace_stack(size_t *sp)
{
    printk("stack:\n");
    for (int i = 0; i < 8; i++) {
        printk(" [%p]: ", sp);
        for (int i = 0; i < 4; i++)
            printk("%08lx ", *sp++);
        printk("\n");
    }
}

void backtrace_call(struct regs *regs)
{
    struct stackframe *frame;
    size_t sp, fp, pc;
    size_t min, max;

    sp = regs->sp;
    fp = regs->l0;
    pc = regs->pc;

    printk("backtrace:\n");
    for (;;) {
        min = sp;
        max = align_high(sp, THREAD_SIZE);

        printk(" [0x%08lx]: \n", pc);
		if (unlikely(fp < min || max < fp || fp & 0x3))
			break;

        frame = (struct stackframe *)fp;
        sp = fp;
        fp = frame->fp;
        pc = frame->pc;
    }
}

void backtrace(struct regs *regs)
{
    backtrace_regs(regs);
    backtrace_stack((size_t *)regs->sp);
    backtrace_call(regs);
}
