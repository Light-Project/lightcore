/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _ASM_GENERIC_PGTABLE_P4D_H_
#define _ASM_GENERIC_PGTABLE_P4D_H_

#include <asm/pgtable.h>

typedef struct { pgd_t pgd; } p4d_t;

#define P4D_SHIFT       PGDIR_SHIFT
#define P4D_SIZE        (_AC(1,ULL) << P4D_SHIFT)
#define P4D_MASK        (~(P4D_SIZE - 1))
#define PTRS_PER_P4D    1

static inline bool p4d_none(pgd_t *pgd)
{
    return false;
}

static inline p4d_t *p4d_offset(pgd_t *pgd, size_t addr)
{
    return (p4d_t *)pgd;
}

static inline size_t p4d_bound_size(size_t addr, size_t size)
{
    return size;
}

static inline state p4d_alloc(pgd_t *pgd, size_t addr)
{
    return -ENOERR;
}

static inline state p4d_alloc_entry(pgd_t *pgd, size_t addr)
{
    return -ENOERR;
}

#endif  /* _ASM_GENERIC_PGTABLE_P4D_H_ */
