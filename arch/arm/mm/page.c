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

struct pgd page_dir[4096] __aligned(0x4000);
struct pte pt_himem[himem_pts][PTRS_PER_PTE] __aligned(0x400);

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

static void pde_set(int index, bool huge, phys_addr_t pa_pte, bool user)
{
    page_dir[index].p           = true;
    page_dir[index].c           = PAGE_C_CACHEON;
    page_dir[index].b           = PAGE_B_BACK;

    if (huge) {
        page_dir[index].type    = PDE_TYPE_SECTION;
        page_dir[index].ap      = PAGE_AP_SYSTEM;
        page_dir[index].section = pa_pte >> PGDIR_SHIFT;
    } else {
        page_dir[index].type    = PDE_TYPE_COARSE;
        page_dir[index].coarse  = pa_pte >> PGDIR_COARSE_SHIFT;
    }
}

static void pte_set(size_t va, struct pte *val)
{
    struct pgd *pgd;
    struct pte *pte;
    size_t index;

    index = pde_index(va);
    pgd = &page_dir[index];

    pte = pa_to_va((pgd->coarse << PGDIR_COARSE_SHIFT));

    index = pte_index(va);
    pte[index] = *val;
}

state arch_page_map(size_t pa, size_t va, size_t size)
{
    size_t count = size;
    struct pte pte;

    if (!(page_aligned(va) && page_aligned(size)))
        return -EINVAL;

    for (count >>= PAGE_SHIFT; count; --count) {
        pte.val = PTE_TYPE_SMALL;
        pte.c = PAGE_C_CACHEON;
        pte.b = PAGE_B_BACK;
        pte.small = pa >> PAGE_SHIFT;

        pte.ap  = PAGE_AP_SYSTEM;
        pte.ap1 = PAGE_AP_SYSTEM;
        pte.ap2 = PAGE_AP_SYSTEM;
        pte.ap3 = PAGE_AP_SYSTEM;

        pte_set(va, &pte);

        va += PAGE_SIZE;
        pa += PAGE_SIZE;
    }

    dcache_writeback_all();
    tlb_inval_range(va, size);

    return -ENOERR;
}

void arch_page_setup(void)
{
    phys_addr_t pa = CONFIG_RAM_BASE;
    uint32_t c = 0;

    /* Mapping kernel space directly */
    for(int index = kernel_index; index < himem_index; ++index) {
        pde_set(index, true, pa, 0);
        pa += PGDIR_SIZE;
    }

    /* Loading himem pte */
    for(int index = himem_index; index < PTRS_PER_PGD; ++index) {
        pa = va_to_pa(&pt_himem[c++][0]);
        pde_set(index, false, pa, 0);
    }

    cp15_set(c3, c0, 0, (DOMAIN_CLIENT << 2) | DOMAIN_CLIENT);

    dcache_writeback_all();
    ttb_set(va_to_pa(page_dir));
    tlb_inval_all();
}
