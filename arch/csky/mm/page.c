/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <mm.h>
#include <asm/pgtable.h>
#include <asm/cache.h>
#include <asm/tlb.h>
#include <asm/mmu.h>

#define kernel_index    (CONFIG_PAGE_OFFSET >> PGDIR_SHIFT)
#define himem_index     (CONFIG_HIGHMAP_OFFSET >> PGDIR_SHIFT)
#define himem_pts       ((VIRTS_SIZE - CONFIG_HIGHMAP_OFFSET) >> PGDIR_SHIFT)

struct pgd page_dir[PTRS_PER_PGD] __aligned(PAGE_SIZE);
struct pte pt_himem[himem_pts][PTRS_PER_PTE] __aligned(PAGE_SIZE);

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
 *
 */

static void pte_set(size_t va, struct pte *val)
{
    struct pgd *pgd;
    struct pte *pte;
    size_t index;

    index = pde_index(va);

    pgd = &page_dir[index];
    pte = pa_to_va(pgd->pte);

    index = pte_index(va);
    pte[index] = *val;
}

state arch_page_map(size_t pa, size_t va, size_t size)
{
    struct pte pte;

    if (!(page_aligned(va) && page_aligned(size)))
        return -EINVAL;

    for (size >>= PAGE_SHIFT; size; --size) {
        pte.val = 0;
        pte.g = true;
        pte.cache = PAGE_CACHE;
        pte.addr = pa >> PAGE_SHIFT;

        pte_set(va, &pte);

        va += PAGE_SIZE;
        pa += PAGE_SIZE;
    }

    dcache_writeback_all();
    tlb_inval_all();

    return -ENOERR;
}

void __init arch_page_setup(void)
{
    phys_addr_t pa = 0;
    uint32_t val = 0;

    /* Mapping kernel space directly */
    mmu_msa0_set(CONFIG_RAM_BASE |
                 MSA_CACHE | MSA_DIRTY | MSA_VAILD);
    mmu_msa1_set((CONFIG_RAM_BASE + 0x20000000) |
                 MSA_CACHE | MSA_DIRTY | MSA_VAILD);

    /* Loading himem pte */
    for(int index = himem_index; index < PTRS_PER_PGD; ++index) {
        pa = va_to_pa(&pt_himem[val++][0]);
        page_dir[index].pte = pa;
    }

    tlb_inval_all();
    icache_inval_all();

    mmu_pagemask_set(0);

    mmu_pgd_set(va_to_pa(page_dir) | MPGD_HRE);
    mmu_entryhi_set(0);
}

