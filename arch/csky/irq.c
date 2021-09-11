/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <irq.h>
#include <asm/irq.h>
#include <asm/entry.h>
#include <asm/regs.h>

static void *irq_table[IRQ_NR_MAX] __aligned(1024) = {
    [VEC_RESET] = entry_generic_interrupt,
    [VEC_ALIGN] = entry_generic_interrupt,
    [VEC_ACCESS] = entry_generic_interrupt,
    [VEC_ZERODIV] = entry_generic_interrupt,
    [VEC_ILLEGAL] = entry_generic_interrupt,
    [VEC_PRIV] = entry_generic_interrupt,
    [VEC_TRACE] = entry_generic_interrupt,
    [VEC_BREAKPOINT] = entry_generic_interrupt,
    [VEC_SOFTRESET] = entry_generic_interrupt,
    [VEC_AUTOVEC] = entry_generic_interrupt,
    [VEC_FAUTOVEC] = entry_generic_interrupt,
    [VEC_HWACCEL] = entry_generic_interrupt,
    [VEC_TLBMISS] = entry_generic_interrupt,
    [VEC_TRAP0] = NULL,
    [VEC_TLBMODIFIED] = entry_page_fault,
    [VEC_TLBINVALIDL] = entry_page_fault,
    [VEC_TLBINVALIDS] = entry_page_fault,
};

void generic_interrupt(irqnr_t vector)
{
    irq_handle(vector);
}

void __init arch_irq_init(void)
{
    mtcr("vbr", irq_table);
}

