/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _ASM_GENERIC_PGTABLE_PMD_H_
#define _ASM_GENERIC_PGTABLE_PMD_H_

#include <asm-generic/pgtable-pud.h>

typedef struct { pud_t pud; } pmd_t;

#define PMD_SHIFT       PUD_SHIFT
#define PMD_SIZE        (_AC(1,ULL) << PMD_SHIFT)
#define PMD_MASK        (~(PMD_SHIFT - 1))
#define PTRS_PER_PMD    1

static inline pmd_t *pmd_offset(pud_t *pgd, size_t addr)
{
    return (pmd_t *)pgd;
}

static inline size_t pmd_bound_size(size_t addr, size_t size)
{
    return size;
}

#endif  /* _ASM_GENERIC_PGTABLE_PMD_H_ */
