/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _ASM_GENERIC_PGTABLE_NOP4D_H_
#define _ASM_GENERIC_PGTABLE_NOP4D_H_

#include <types.h>
#include <stddef.h>
#include <state.h>

typedef pgd_t p4d_t;

#define P4D_SHIFT       PGDIR_SHIFT
#define P4D_SIZE        (_AC(1,ULL) << P4D_SHIFT)
#define P4D_MASK        (~(P4D_SIZE - 1))
#define PTRS_PER_P4D    1

#define p4d_bound_size p4d_bound_size
static inline size_t p4d_bound_size(size_t addr, size_t size)
{
    return size;
}

#define pgd_get_present pgd_get_present
static inline bool pgd_get_present(pgd_t *pgd)
{
    return true;
}

#define pgd_inval pgd_inval
static inline bool pgd_inval(pgd_t *pgd)
{
    return false;
}

#define pgd_clear pgd_clear
static inline void pgd_clear(pgd_t *pgd)
{

}

#define p4d_offset p4d_offset
static inline p4d_t *p4d_offset(pgd_t *pgd, size_t addr)
{
    return (p4d_t *)pgd;
}

#define pgalloc_p4d pgalloc_p4d
static inline state pgalloc_p4d(pgd_t *pgd, size_t addr)
{
    return -ENOERR;
}

#define pgfree_p4d pgfree_p4d
static inline void pgfree_p4d(pgd_t *pgd, size_t addr)
{

}

#define p4d_alloc p4d_alloc
static inline state p4d_alloc(pgd_t *pgd, size_t addr)
{
    return -ENOERR;
}

#endif  /* _ASM_GENERIC_PGTABLE_P4D_H_ */
