/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <linkage.h>
#include <kernel.h>
#include <printk.h>

#include <asm/proc.h>
#include <asm/traps.h>
#include <asm/backtrace.h>

asmlinkage __visible void trap_general_protection(struct regs *regs)
{
    recall("general protection", regs);
    panic("general protection");
}

asmlinkage __visible void trap_double_fault(struct regs *regs)
{
    panic("double fault irreparable");
}

