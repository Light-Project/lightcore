/* SPDX-License-Identifier: GPL-2.0-or-later */
#include <system/irq.h>
#include <printk.h>

#include <asm/irq.h>
#include <asm/entry.h>
#include <asm/regs.h>

void generic_interrupt(irqnr_t vector)
{
    irq_handle(vector);
}
