/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _ASM_GENERIC_PGALLOC_NOPMD_H_
#define _ASM_GENERIC_PGALLOC_NOPMD_H_

#include <asm-generic/pgtable-nopmd.h>
#include <asm-generic/pgalloc-nopud.h>

#define pgalloc_pmd pgalloc_pmd
static inline state pgalloc_pmd(struct memory *mem, uintptr_t addr)
{
    return -ENOERR;
}

#define pgfree_pmd pgfree_pmd
static inline void pgfree_pmd(struct memory *mem, pmd_t *pmd)
{
    /* Nothing */
}

#define pmd_alloc pmd_alloc
static inline state pmd_alloc(struct memory *mem, pud_t *pud, uintptr_t addr)
{
    return -ENOERR;
}

#define pmd_free pmd_free
static inline void pmd_free(struct memory *mem, pud_t *pud, uintptr_t addr)
{
    /* Nothing */
}

#endif /* _ASM_GENERIC_PGALLOC_NOPMD_H_ */
