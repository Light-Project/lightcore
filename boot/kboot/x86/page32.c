/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <kboot.h>
#include <size.h>
#include <asm/regs.h>
#include <asm/page.h>
#include <arch/x86/page.h>

struct pgd page_dir[PTRS_PER_PGD] __aligned(0x1000);

static __always_inline void pgd_huge_set(int index, phys_addr_t pa)
{
    page_dir[index].p     = true;
    page_dir[index].rw    = PAGE_RW_RW;
    page_dir[index].ps    = PAGE_PS_HUGE;
    page_dir[index].g     = true;
    page_dir[index].addrl = pa >> PGDIR_SHIFT;
}

static __always_inline void section_link(uintptr_t va, phys_addr_t pa, size_t size)
{
    int index = va >> PGDIR_SHIFT;

    for (size >>= PGDIR_SHIFT; size; --size) {
        pgd_huge_set(index++, pa);
        pa += PGDIR_SIZE;
    }
}

void kernel_map(void)
{
    uint32_t val;

    section_link(0x00000000, CONFIG_RAM_BASE, SZ_1MiB * 16);
    section_link(CONFIG_PAGE_OFFSET, CONFIG_RAM_BASE, CONFIG_HIGHMEM_OFFSET);

    val = cr4_get();
    val |= CR4_PSE;
    cr4_set(val);

    val = cr3_get();
    val &= ~0xfffff000;
    val |= (uintptr_t)page_dir;
    cr3_set(val);

    val = cr0_get();
    val |= CR0_PG;
    val &= ~(CR0_CD | CR0_NW);
    cr0_set(val);
}
