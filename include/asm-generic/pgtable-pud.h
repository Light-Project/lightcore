/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _ASM_GENERIC_PGTABLE_PUD_H_
#define _ASM_GENERIC_PGTABLE_PUD_H_

#include <asm-generic/pgtable-p4d.h>

typedef struct { p4d_t p4d; } pud_t;

#define PUD_SHIFT       P4D_SHIFT
#define PUD_SIZE        (_AC(1,ULL) << PUD_SHIFT)
#define PUD_MASK        (~(PUD_SHIFT - 1))
#define PTRS_PER_PMD    1

static inline pud_t *pud_offset(p4d_t *pgd, size_t addr)
{
    return (pud_t *)pgd;
}

static inline size_t pud_bound_size(size_t addr, size_t size)
{
    return size;
}

#endif  /* _ASM_GENERIC_PGTABLE_PUD_H_ */
