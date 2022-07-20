/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <linkage.h>
#include <syscall.h>
#include <asm/regs.h>

asmlinkage __visible void arch_syscall_80(struct regs *regs)
{
    long retval;

    retval = syscall_handle(
        (unsigned long)regs->ax,
        (long)regs->bx, (long)regs->cx,
        (long)regs->dx, (long)regs->si,
        (long)regs->di, (long)regs->bp
    );

    regs->ax = retval;
}
