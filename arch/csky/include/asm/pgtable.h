/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _ASM_CSKY_PGALLOC_H_
#define _ASM_CSKY_PGALLOC_H_

#include <types.h>
#include <state.h>
#include <arch/csky/page.h>
#include <asm/page.h>

typedef struct pte pte_t;
typedef struct pgd pgd_t;

#define SSEG0_BASE  0x80000000
#define SSEG1_BASE  0xa0000000

#include <asm-generic/pgtable-nopmd.h>

static inline bool pmd_get_present(pmd_t *pmd)
{
    return !!pmd->pte;
}

#define pte_index(va) (((va) >> PAGE_SHIFT) & (PTRS_PER_PTE - 1))
#define pde_index(va) ((va) >> PGDIR_SHIFT)

state arch_page_map(size_t pa, size_t va, size_t size);
void arch_page_setup(void);

#endif /* _ASM_CSKY_PGALLOC_H_ */
