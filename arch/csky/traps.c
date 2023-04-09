/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 John Sanpe <sanpeqf@gmail.com>
 */

#include <linkage.h>
#include <kernel.h>
#include <printk.h>
#include <asm/backtrace.h>
#include <asm/proc.h>

asmlinkage __visible void trap_misaligned(struct regs *regs)
{
    backtrace(regs);
    panic("unaligned memory access exception");
}

asmlinkage __visible void trap_buserr(struct regs *regs)
{
    backtrace(regs);
    panic("access error exception");
}

asmlinkage __visible void trap_zerodiv(struct regs *regs)
{
    backtrace(regs);
    panic("divide zero exception");
}

asmlinkage __visible void trap_illinsn(struct regs *regs)
{
    backtrace(regs);
    panic("illegal instruction exception");
}

asmlinkage __visible void trap_priv(struct regs *regs)
{
    backtrace(regs);
}
