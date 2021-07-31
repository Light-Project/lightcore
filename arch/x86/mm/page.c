/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <string.h>
#include <mm.h>

#include <asm/tlb.h>
#include <asm/regs.h>
#include <asm/page.h>
#include <asm/pgalloc.h>
#include <asm/pgtable.h>

struct pde page_dir[PTRS_PER_PGD] __aligned(0x1000);
struct pte pt_himem[64][PTRS_PER_PTE] __aligned(0x1000);

#define kernel_index    (CONFIG_PAGE_OFFSET >> PGDIR_SHIFT)
#define himem_index     (CONFIG_HIGHMAP_OFFSET >> PGDIR_SHIFT)

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

static void pde_set(int index, bool huge, phys_addr_t pa_pte, bool user)
{
    page_dir[index].p       = true;
    page_dir[index].rw      = PAGE_RW_RW;
    page_dir[index].pwt     = PAGE_PWT_BACK;
    page_dir[index].pcd     = PAGE_PCD_CACHEON;
    page_dir[index].g       = true;
    page_dir[index].us      = user;

    if (!huge) {
        page_dir[index].ps      = PAGE_PS_4K;
        page_dir[index].addr    = pa_pte >> PAGE_SHIFT;
    } else {
        page_dir[index].ps      = PAGE_PS_4M;
        page_dir[index].addrl   = pa_pte >> PGDIR_SHIFT;
    }
}

/**
 * pte_set - set page table entry
 * @addr: virtual address
 * @val: PTE filling content
 * 
 */
static void pte_set(size_t va, struct pte *pte_val)
{
    struct pde *pde;
    struct pte *pte;
    size_t index;

    index = pde_index(va);
    pde = &page_dir[index];

    /* Without PTE. */
    if ((!pde->p) || pde->ps)
        return;

    pte = pa_to_va((pde->addr << PAGE_SHIFT));

    index = pte_index(va);
    pte[index] = *pte_val;
}

/**
 * arch_page_map - map 
 */
state arch_page_map(size_t pa, size_t va, size_t size)
{
    struct pte pte;

    if(!(page_aligned(va) && page_aligned(va)))
        return -EINVAL;

    for(size >>= PAGE_SHIFT; size; --size) {
        pte.val = 0x1;
        pte.us = 0;
        pte.addr = pa >> PAGE_SHIFT;
        pte_set(va, &pte);
    }

    tlb_flush_all();

    return -ENOERR;
}

void *arch_page_alloc_user(void)
{
    return kzalloc(kernel_index - 1, GFP_KERNEL);
}

/**
 * arch_page_switch - Toggle user section in global page table
 * @ptd_p: Page directory of the previous process
 * @ptd_n: Page directory of the next process
 * 
 */
void arch_page_switch(void *ptd_p, void *ptd_n)
{
    if(ptd_p)
        memcpy(page_dir, ptd_p, sizeof(struct pde) * kernel_index);
    memcpy(ptd_n, page_dir, sizeof(struct pde) * kernel_index);
}

/**
 * arch_page_setup - Initialize global page table
 */
void __init arch_page_setup(void)
{    
    phys_addr_t pa = 0;
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

    val = cr4_get();
    val |= CR4_PSE;
    cr4_set(val);

    val = cr3_get();
    val &= ~0xfffff000;
    val |= va_to_pa(&page_dir) & 0xfffff000;
    cr3_set(val);
}
