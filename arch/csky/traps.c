/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <linkage.h>
#include <power.h>
#include <printk.h>

#include <asm/traps.h>
#include <asm/recall.h>

asmlinkage __visible void trap_general_protection(struct regs *regs)
{

    recall("general protection", regs);
    panic("general protection");
    power_halt();
}
