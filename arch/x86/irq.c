/* SPDX-License-Identifier: GPL-2.0-or-later */
#include <irq.h>
#include <printk.h>

#include <asm/irq.h>
#include <asm/entry.h>
#include <asm/regs.h>

asmlinkage __visible void generic_interrupt(unsigned long irqnr)
{
    irq_handle(irqnr);
}
