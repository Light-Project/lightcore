/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <string.h>
#include <mm.h>

#include <asm/page.h>
#include <asm/pgtable.h>
#include <asm/tlb.h>
#include <asm/regs.h>
#include <asm/mmu.h>

#define kernel_index    (CONFIG_PAGE_OFFSET >> PGDIR_SHIFT)
#define himem_index     (CONFIG_HIGHMAP_OFFSET >> PGDIR_SHIFT)
#define himem_pts       ((VIRTS_SIZE - CONFIG_HIGHMAP_OFFSET) >> PGDIR_SHIFT)

struct pde page_dir[4096] __aligned(0x4000);
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
    page_dir[index].b       = PAGE_B_BACK;
    page_dir[index].c       = PAGE_C_CACHEON;

    if (!huge) {
        page_dir[index].type    = PAGE_TYPE_COARSE;
        page_dir[index].coarse  = pa_pte >> SECTION_SHIFT;
    } else {
        page_dir[index].type    = PAGE_TYPE_SECTION;
        page_dir[index].ap      = 0x03;
        page_dir[index].section = pa_pte >> SECTION_SHIFT;
    }
}

static void pte_set(size_t va, struct pte *val)
{
    struct pde *pde;
    struct pte *pte;
    size_t index;

    index = pde_index(va);
    pde = &page_dir[index];

    pte = pa_to_va((pde->coarse << COARSE_SHIFT));

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
        pte.b = PAGE_B_BACK;
        pte.c = PAGE_C_CACHEON;
        pte.small = pa >> PAGE_SHIFT;

        pte_set(va, &pte);

        va += PAGE_SIZE;
        pa += PAGE_SIZE;
    }

    tlb_inval_all();
    return -ENOERR;
}
void arch_page_setup(void)
{
    phys_addr_t pa = CONFIG_RAM_BASE;
    uint32_t val = 0;

    /* Mapping kernel space directly */
    for(int index = kernel_index; index < himem_index; ++index) {
        pde_set(index, true, pa, 0);
        pa += 1 << PGDIR_SHIFT;
    }

    /* Loading himem pte */
    for(int index = himem_index; index < PTRS_PER_PGD; ++index) {
        pa = va_to_pa(&pt_himem[val++][0]);
        pde_set(index, false, pa, 0);
    }

    ttb_set(va_to_pa(page_dir));
    tlb_inval_all();
}
