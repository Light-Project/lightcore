/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _ASM_GENERIC_PGALLOC_NOPUD_H_
#define _ASM_GENERIC_PGALLOC_NOPUD_H_

#include <asm-generic/pgtable-nopud.h>
#include <asm-generic/pgalloc-nop4d.h>

#define pgalloc_pud pgalloc_pud
static inline state pgalloc_pud(struct memory *mem, uintptr_t addr)
{
    return -ENOERR;
}

#define pgfree_pud pgfree_pud
static inline void pgfree_pud(struct memory *mem, pud_t *pud)
{
    /* Nothing */
}

#define pud_alloc pud_alloc
static inline state pud_alloc(struct memory *mem, p4d_t *p4d, uintptr_t addr)
{
    return -ENOERR;
}

#define pud_free pud_free
static inline void pud_free(struct memory *mem, p4d_t *p4d, uintptr_t addr)
{
    /* Nothing */
}

#endif /* _ASM_GENERIC_PGALLOC_NOPUD_H_ */
