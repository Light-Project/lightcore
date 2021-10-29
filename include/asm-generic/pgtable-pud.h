/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _ASM_GENERIC_PGTABLE_PUD_H_
#define _ASM_GENERIC_PGTABLE_PUD_H_

#include <asm-generic/pgtable-p4d.h>

typedef struct { p4d_t p4d; } pud_t;

#define PUD_SHIFT       P4D_SHIFT
#define PUD_SIZE        (_AC(1,ULL) << PUD_SHIFT)
#define PUD_MASK        (~(PUD_SHIFT - 1))
#define PTRS_PER_PMD    1

static inline bool pud_none(pgd_t *pgd)
{
    return false;
}

static inline pud_t *pud_offset(p4d_t *pgd, size_t addr)
{
    return (pud_t *)pgd;
}

static inline size_t pud_bound_size(size_t addr, size_t size)
{
    return size;
}

static inline state pud_alloc(pgd_t *pgd, size_t addr)
{
    return -ENOERR;
}

static inline state pud_alloc_entry(pgd_t *pgd, size_t addr)
{
    return -ENOERR;
}

#endif  /* _ASM_GENERIC_PGTABLE_PUD_H_ */
