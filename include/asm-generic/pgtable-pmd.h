/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _ASM_GENERIC_PGTABLE_PMD_H_
#define _ASM_GENERIC_PGTABLE_PMD_H_

#include <asm-generic/pgtable-pud.h>

typedef pud_t pmd_t;

#define PMD_SHIFT       PUD_SHIFT
#define PMD_SIZE        (_AC(1,ULL) << PMD_SHIFT)
#define PMD_MASK        (~(PMD_SHIFT - 1))
#define PTRS_PER_PMD    1

static inline bool pud_get_present(pud_t *pud)
{
    return true;
}

static inline state pmd_alloc(pud_t *pud, size_t addr)
{
    return -ENOERR;
}

static inline state pmd_alloc_entry(pud_t *pud, size_t addr)
{
    return -ENOERR;
}

static inline pmd_t *pmd_offset(pud_t *pud, size_t addr)
{
    return (pmd_t *)pud;
}

#define pmd_bound_size pmd_bound_size
static inline size_t pmd_bound_size(size_t addr, size_t size)
{
    return size;
}

static inline state pmd_set_huge(pmd_t *pmd, phys_addr_t addr)
{
    return 0;
}

#endif  /* _ASM_GENERIC_PGTABLE_PMD_H_ */
