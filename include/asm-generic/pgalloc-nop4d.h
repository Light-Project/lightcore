/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _ASM_GENERIC_PGALLOC_NOP4D_H_
#define _ASM_GENERIC_PGALLOC_NOP4D_H_

#include <asm-generic/pgtable-nop4d.h>

struct memory;

#define pgalloc_p4d pgalloc_p4d
static inline state pgalloc_p4d(struct memory *mem, uintptr_t addr)
{
    return -ENOERR;
}

#define pgfree_p4d pgfree_p4d
static inline void pgfree_p4d(struct memory *mem, pgd_t *p4d)
{
    /* Nothing */
}

#define p4d_alloc p4d_alloc
static inline state p4d_alloc(struct memory *mem, pgd_t *pgd, uintptr_t addr)
{
    return -ENOERR;
}

#define p4d_free p4d_free
static inline void p4d_free(struct memory *mem, pgd_t *pgd, uintptr_t addr)
{
    /* Nothing */
}

#endif /* _ASM_GENERIC_PGALLOC_NOP4D_H_ */
