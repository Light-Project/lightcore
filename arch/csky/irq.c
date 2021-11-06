/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <linkage.h>
#include <irq.h>
#include <asm/irq.h>
#include <asm/entry.h>
#include <asm/regs.h>

static void (*irq_table[IRQ_NR_MAX])(void) __aligned(1024) = {
    [VEC_RESET]         = entry_generic_interrupt,
    [VEC_ALIGN]         = entry_misaligned,
    [VEC_ACCESS]        = entry_buserr,
    [VEC_ZERODIV]       = entry_zerodiv,
    [VEC_ILLEGAL]       = entry_illinsn,
    [VEC_PRIV]          = entry_priv,
    [VEC_TRACE]         = entry_generic_interrupt,
    [VEC_BREAKPOINT]    = entry_generic_interrupt,
    [VEC_SOFTRESET]     = entry_generic_interrupt,
    [VEC_AUTOVEC]       = entry_generic_interrupt,
    [VEC_FAUTOVEC]      = entry_generic_interrupt,
    [VEC_HWACCEL]       = entry_generic_interrupt,
    [VEC_TLBMISS]       = entry_generic_interrupt,
    [VEC_TLBMODIFIED]   = entry_pagefault,
    [VEC_TRAP0]         = entry_syscall,
    [VEC_TRAP1]         = entry_syscall,
    [VEC_TRAP2]         = entry_syscall,
    [VEC_TRAP3]         = entry_syscall,
    [VEC_TLBINVALIDL]   = entry_pagefault,
    [VEC_TLBINVALIDS]   = entry_pagefault,
};

asmlinkage void generic_interrupt(irqnr_t vector)
{
    panic("generic interrupt");
    irq_handle(vector);
}

void __init arch_irq_init(void)
{
    mtcr("vbr", irq_table);
}

