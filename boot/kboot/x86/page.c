/* SPDX-License-Identifier: GPL-2.0-or-later */
#include <kboot.h>
#include <size.h>

#include <asm/page.h>
#include <arch/x86/page.h>
#include <asm/regs.h>

struct pde page_dir[1024] __aligned(0x1000);

static void pde_huge_set(int index, phys_addr_t pa, bool user)
{
    page_dir[index].p       = true;
    page_dir[index].rw      = PAGE_RW_RW;
    page_dir[index].us      = user;
    page_dir[index].pwt     = PAGE_PWT_BACK;
    page_dir[index].pcd     = PAGE_PCD_CACHEON;
    page_dir[index].ps      = PAGE_PS_4M;
    page_dir[index].g       = true;
    page_dir[index].addrl   = pa >> PGDIR_SHIFT;
}

static void section_link(uint32_t va, uint32_t pa, uint32_t size)
{
    int index = va >> PGDIR_SHIFT;

    for(size >>= PGDIR_SHIFT; size > 0; --size) {
        pde_huge_set(index++, pa, 0);
        pa += 1 << PGDIR_SHIFT;
    }
}

void kernel_map()
{
    uint32_t val;

    section_link(0x00000000, 0x00000000, (uint32_t)size_1MiB * 16);
    section_link(CONFIG_PAGE_OFFSET, 0x00000000, size_1MiB * 768);

    val = cr4_get();
    val |= CR4_PSE;
    cr4_set(val);

    val = cr3_get();
    val &= ~0xfffff000;
    val |= (size_t)&page_dir & 0xfffff000;
    cr3_set(val);

    val = cr0_get();
    val |= 1 << 31;
    cr0_set(val);
}
