/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <mm/pgtable.h>
#include <asm/regs.h>

#define kernel_index    (CONFIG_PAGE_OFFSET >> PGDIR_SHIFT)
#define himem_index     (CONFIG_HIGHMAP_OFFSET >> PGDIR_SHIFT)

pgd_t page_dir[PTRS_PER_PGD] __aligned(0x1000);

/*
 *  Kernel page table
 *
 *   Addr | 0                   3G         3.7G
 *        | ####################|+++++++++++|#######
 *  Space |        User         ^  Kernel   ^ Himem
 *  Index |               kernel_index  himem_index
 *
 *  User:   This part is blank
 *  Kernel: Use fixed huge page mapping
 *  Himem:  Use statically assigned pt_himem
 *
 */

void __init arch_page_setup(void)
{
    phys_addr_t addr = CONFIG_RAM_BASE;
    uint32_t index, val;

    /* Mapping kernel space directly */
    for (index = kernel_index; index < himem_index; ++index) {
        pmd_set_huge(&page_dir[index], addr, 0);
        addr += PMD_SIZE;
    }

    val = cr4_get();
    val |= CR4_PSE;
    cr4_set(val);

    cr3_set(va_to_pa(&page_dir) & CR3_PDB);
}
