/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <asm/regs.h>
#include <asm/mmu.h>
#include <asm/tlb.h>

void tlb_flush_all(void)
{
    mmu_mcir_write(MCIR_TLBINVA);
}

