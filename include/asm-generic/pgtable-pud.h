/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _ASM_GENERIC_PGTABLE_PUD_H_
#define _ASM_GENERIC_PGTABLE_PUD_H_

#include <asm-generic/pgtable-p4d.h>

typedef p4d_t pud_t;

#define PUD_SHIFT       P4D_SHIFT
#define PUD_SIZE        (_AC(1,ULL) << PUD_SHIFT)
#define PUD_MASK        (~(PUD_SHIFT - 1))
#define PTRS_PER_PMD    1

static inline bool p4d_get_present(p4d_t *pud)
{
    return true;
}

static inline state pud_alloc(p4d_t *p4d, size_t addr)
{
    return -ENOERR;
}

static inline state pud_alloc_entry(p4d_t *p4d, size_t addr)
{
    return -ENOERR;
}

static inline pud_t *pud_offset(p4d_t *p4d, size_t addr)
{
    return (pud_t *)p4d;
}

#define pud_bound_size pud_bound_size
static inline size_t pud_bound_size(size_t addr, size_t size)
{
    return size;
}

static inline state pud_set_huge(pud_t *pud, phys_addr_t addr)
{
    return 0;
}

#endif  /* _ASM_GENERIC_PGTABLE_PUD_H_ */
