/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <stddef.h>
#include <asm/pgtable.h>
#include <asm/cache.h>
#include <asm/tlb.h>
#include <asm/regs.h>
#include <asm/mmu.h>

struct pde page_dir[4096] __aligned(0x4000);

static void pde_huge_set(int index, phys_addr_t pa)
{
    page_dir[index].p       = true;
    page_dir[index].c       = PAGE_C_CACHEON;
    page_dir[index].b       = PAGE_B_BACK;
    page_dir[index].ap      = PAGE_AP_SYSTEM;
    page_dir[index].type    = PDE_TYPE_SECTION;
    page_dir[index].section = pa >> PGDIR_SHIFT;
}

static void section_link(size_t va, phys_addr_t pa, size_t size)
{
    int index = va >> PGDIR_SHIFT;

    for(size >>= PGDIR_SHIFT; size; --size) {
        pde_huge_set(index++, pa);
        pa += PGDIR_SIZE;
    }
}

void kernel_map(void)
{
    section_link(CONFIG_RAM_BASE, CONFIG_RAM_BASE, CONFIG_HIGHMEM_OFFSET);
    section_link(CONFIG_PAGE_OFFSET, CONFIG_RAM_BASE, CONFIG_HIGHMEM_OFFSET);

    cp15_set(c3, c0, 0, 0x03);

    tlb_inval_all();
    ttb_set((phys_addr_t)page_dir);
    mmu_enable();

    icache_enable();
    dcache_enable();
}
