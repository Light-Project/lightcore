/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <string.h>
#include <mm.h>

#include <asm/page.h>
#include <asm/tlb.h>
#include <asm/regs.h>

struct pde page_dir[PTRS_PER_PGD] __aligned(0x1000);
struct pte pt_himem[64][PTRS_PER_PTE] __aligned(0x1000);

#define kernel_index    (CONFIG_PAGE_OFFSET >> PGDIR_SHIFT)
#define himem_index     ((size_t)pa_to_va(CONFIG_HIGHMEM_OFFSET) >> PGDIR_SHIFT)

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
    page_dir[index].us      = user;
    page_dir[index].pwt     = PAGE_PWT_BACK;
    page_dir[index].pcd     = PAGE_PCD_CACHEON;
    page_dir[index].ps      = PAGE_PS_4M;
    page_dir[index].g       = true;

    if(!huge)
        page_dir[index].addrl   = pa_pte >> PGDIR_SHIFT;
    else
        page_dir[index].addrl   = pa_pte >> PGDIR_SHIFT;
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

    pde = &page_dir[pde_index(va)];

    /* Without PTE. */
    if((!pde->p)||pde->ps)
        return;

    pte = (struct pte *)(pde->addr << PAGE_SHIFT);

    pte[pte_index(va)] = *pte_val;
}

/**
 * arch_page_map - map 
 */
void arch_page_map(struct vm_area *va, gfp_t gfp)
{
    struct pte pte;
    size_t start_addr, last_addr;
    uint8_t flags;

    start_addr = (size_t)va->addr;
    last_addr = start_addr + va->size;

    if(gfp & GFP_HIGHMEM) { /* Disable mapping direct memory */
        if((size_t)va->addr < CONFIG_HIGHMEM_OFFSET)
            return;
        flags = 0;
    } else { /* Map user space */
        flags = 1;
    }

    while(start_addr >= last_addr) {
        pte.val = 0x1;
        pte.us = flags;
        pte.addr = va->phys_addr >> PAGE_SHIFT;
        pte_set((size_t)va, &pte);
        start_addr += PAGE_SHIFT;
    }

    tlb_flush_all();
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
    tlb_flush_all();
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
