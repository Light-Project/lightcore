/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <asm/page.h>
#include <asm/pgalloc.h>
#include <asm/regs.h>
#include <asm/tlb.h>
#include <asm/mmu.h>

/*
 *  Kernel page table
 *
 *   Addr | 0               2G      3G
 *        | ################|++++++++|########
 *  Space |      User       ^ Kernel ^ Himem
 * 
 *  User:   This part is blank
 *  Kernel: Use fixed huge page mapping
 *  Himem:  Use statically assigned pt_himem
 */

state arch_page_map(size_t pa, size_t va, size_t size)
{

    return -ENOERR;
}

void __init arch_page_setup(void)
{    
    // phys_addr_t pa = 0;
    // uint32_t val = 0;

    /* Mapping kernel space directly */
    mmu_msa0_write(0x00000000 | MSA_CACHE | MSA_DIRTY | MSA_VAILD);
    mmu_msa1_write(0x20000000 | MSA_CACHE | MSA_DIRTY | MSA_VAILD);

    tlb_flush_all();

    // /* Loading himem pte */
    // for(int index = himem_index; index < PTRS_PER_PGD; ++index) {
    //     pa = va_to_pa(&pt_himem[val++][0]);
    //     pde_set(index, false, pa, 0);
    // }

    // val = cr4_get();
    // val |= CR4_PSE;
    // cr4_set(val);

    // val = cr3_get();
    // val &= ~0xfffff000;
    // val |= va_to_pa(&page_dir) & 0xfffff000;
    // cr3_set(val);
}

