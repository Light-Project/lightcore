/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <printk.h>
#include <kernel/power.h>

#include <asm/traps.h>
#include <asm/recall.h>

void trap_general_protection(struct regs *regs)
{
    
    recall("general protection", regs);
    panic("general protection");
    power_halt();
}
