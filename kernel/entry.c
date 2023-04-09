/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 John Sanpe <sanpeqf@gmail.com>
 */

#include <linkage.h>
#include <entry.h>
#include <sched.h>

noinline void entry_irqhandle_enter_form_user(struct regs *regs)
{

}

noinline void entry_irqhandle_exit_to_user(struct regs *regs)
{

}

noinline void entry_irqhandle_enter(struct regs *regs)
{
    if (trap_user_mode(regs)) {
        entry_irqhandle_enter_form_user(regs);
        return;
    }
}

noinline void entry_irqhandle_exit(struct regs *regs)
{
    if (trap_user_mode(regs)) {
        entry_irqhandle_exit_to_user(regs);
        return;
    }
    sched_preempt();
}
