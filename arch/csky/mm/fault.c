/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <linkage.h>
#include <asm/page.h>
#include <asm/mmu.h>
#include <asm/tlb.h>

static void himem_fault(struct regs *regs, size_t addr)
{
    tlb_refresh(addr);
}

asmlinkage void arch_page_fault(struct regs *regs)
{
    size_t addr = mmu_entryhi_get() & PAGE_MASK;

    if (addr > CONFIG_PAGE_OFFSET) {
        himem_fault(regs, addr);
        return;
    }
}
