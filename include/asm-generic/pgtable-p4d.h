/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _ASM_GENERIC_PGTABLE_P4D_H_
#define _ASM_GENERIC_PGTABLE_P4D_H_

#include <asm-generic/pgtable.h>

typedef pgd_t p4d_t;

#define P4D_SHIFT       PGDIR_SHIFT
#define P4D_SIZE        (_AC(1,ULL) << P4D_SHIFT)
#define P4D_MASK        (~(P4D_SIZE - 1))
#define PTRS_PER_P4D    1

static inline bool pgd_get_present(pgd_t *pud)
{
    return true;
}

static inline state p4d_alloc(pgd_t *pgd, size_t addr)
{
    return -ENOERR;
}

static inline state p4d_alloc_entry(pgd_t *pgd, size_t addr)
{
    return -ENOERR;
}

static inline p4d_t *p4d_offset(pgd_t *pgd, size_t addr)
{
    return (p4d_t *)pgd;
}

#define p4d_bound_size p4d_bound_size
static inline size_t p4d_bound_size(size_t addr, size_t size)
{
    return size;
}

static inline state p4d_set_huge(p4d_t *p4d, phys_addr_t addr)
{
    return 0;
}

#endif  /* _ASM_GENERIC_PGTABLE_P4D_H_ */
