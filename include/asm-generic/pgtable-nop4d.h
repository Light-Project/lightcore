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
static inline size_t p4d_bound_size(uintptr_t addr, size_t size)
{
    return size;
}

#define p4d_offset p4d_offset
static inline p4d_t *p4d_offset(pgd_t *pgd, uintptr_t addr)
{
    return (p4d_t *)pgd;
}

/**
 * Typical architecture set definition.
 */

#define pgd_present pgd_present
static inline bool pgd_present(pgd_t *pgd)
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
    /* Nothing */
}

#define pgd_none pgd_none
static inline bool pgd_none(pgd_t *pgd)
{
    return true;
}

#define pgd_get_huge pgd_get_huge
static inline bool pgd_get_huge(pgd_t *pgd)
{
    return false;
}

#endif /* _ASM_GENERIC_PGTABLE_P4D_H_ */
