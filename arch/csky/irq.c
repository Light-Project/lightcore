/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <kernel/irq.h>
#include <asm/irq.h>
#include <asm/entry.h>
#include <asm/regs.h>

static void *irq_table[IRQ_NR_MAX] __aligned(1024) = {
    [RS_FAULT] = entry_generic_interrupt,
    [AG_FAULT] = entry_generic_interrupt,
    [AC_FAULT] = entry_generic_interrupt,
    [DZ_FAULT] = entry_generic_interrupt,
    [IG_FAULT] = entry_generic_interrupt,
    [PV_FAULT] = entry_generic_interrupt,
    [DE_FAULT] = entry_generic_interrupt,
    [BP_FAULT] = entry_generic_interrupt,
    [SR_FAULT] = entry_generic_interrupt,
    [AUTO_INT] = entry_generic_interrupt,
    [FAST_INT] = entry_generic_interrupt,
};

void generic_interrupt(irqnr_t vector)
{
    irq_handle(vector);
}

void __init arch_irq_init(void)
{
    mtcr("vbr", irq_table);
}

