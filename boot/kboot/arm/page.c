/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <kboot.h>
#include <size.h>

#include <arch/arm/page.h>
#include <asm/page.h>
#include <asm/tlb.h>
#include <asm/cache.h>
#include <asm/regs.h>
#include <asm/mmu.h>

struct pde page_dir[4096] __aligned(0x4000);

static void pde_huge_set(int index, phys_addr_t pa)
{
    page_dir[index].type    = PAGE_TYPE_SECTION;
    page_dir[index].b       = PAGE_B_BACK;
    page_dir[index].c       = PAGE_C_CACHEON;
    page_dir[index].ap      = 0x03;
    page_dir[index].section = pa >> SECTION_SHIFT;
}

static void section_link(size_t va, phys_addr_t pa, size_t size)
{
    int index = va >> SECTION_SHIFT;

    for(size >>= SECTION_SHIFT; size; --size) {
        pde_huge_set(index++, pa);
        pa += SECTION_SIZE;
    }
}

void kernel_map(void)
{
    section_link(CONFIG_RAM_BASE, CONFIG_RAM_BASE, CONFIG_HIGHMEM_OFFSET);
    section_link(CONFIG_PAGE_OFFSET, CONFIG_RAM_BASE, CONFIG_HIGHMEM_OFFSET);

    dcache_drain_all();
    tlb_inval_all();
    cp15_set(c3, c0, 0, 0x03);

    ttb_set((phys_addr_t)page_dir);
    mmu_enable();
}
