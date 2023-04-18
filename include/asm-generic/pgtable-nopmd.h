/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _ASM_GENERIC_PGTABLE_NOPMD_H_
#define _ASM_GENERIC_PGTABLE_NOPMD_H_

#include <asm-generic/pgtable-nopud.h>

typedef pud_t pmd_t;

#define PMD_SHIFT       PUD_SHIFT
#define PMD_SIZE        (_AC(1,ULL) << PMD_SHIFT)
#define PMD_MASK        (~(PMD_SHIFT - 1))
#define PTRS_PER_PMD    1

#define pmd_bound_size pmd_bound_size
static inline size_t pmd_bound_size(uintptr_t addr, size_t size)
{
    return size;
}

#define pmd_offset pmd_offset
static inline pmd_t *pmd_offset(pud_t *pud, uintptr_t addr)
{
    return (pmd_t *)pud;
}

/**
 * Typical architecture set definition.
 */

#define pud_present pud_present
static inline bool pud_present(pud_t *pud)
{
    return true;
}

#define pud_inval pud_inval
static inline bool pud_inval(pud_t *pud)
{
    return false;
}

#define pud_clear pud_clear
static inline void pud_clear(pud_t *pud)
{
    /* Nothing */
}

#define pud_none pud_none
static inline bool pud_none(pud_t *pud)
{
    return true;
}

#define pud_get_huge pud_get_huge
static inline bool pud_get_huge(pud_t *pud)
{
    return false;
}

#endif /* _ASM_GENERIC_PGTABLE_PMD_H_ */
