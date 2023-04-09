/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 John Sanpe <sanpeqf@gmail.com>
 */

#include <irqflags.h>
#include <asm/tlb.h>
#include <asm/mmu.h>

void tlb_refresh(size_t addr)
{
    irqflags_t irq;
    uint32_t val, index;

    irq = irq_local_save();

    addr &= MEH_VPN;
    val = mmu_entryhi_get() & MEH_ASID;
    mmu_entryhi_set(addr | val);

    tlb_probe();
    index = mmu_index_get();
    if (index & MIR_P)
        tlb_inval_indexed();

    mmu_entryhi_set(val + 1);
    mmu_entryhi_set(val);

    irq_local_restore(irq);
}
