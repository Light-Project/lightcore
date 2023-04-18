/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _MM_PGALLOC_H_
#define _MM_PGALLOC_H_

#include <mm/page.h>
#include <mm/pgtable.h>
#include <asm/pgalloc.h>

#ifndef pgalloc_pte
static inline pte_t *pgalloc_pte(struct memory *mem, uintptr_t addr)
{
    return get_zeroed_page(GFP_KERNEL);
}

static inline void pgfree_pte(struct memory *mem, pte_t *pte)
{
    free_page(pte);
}
#endif

#ifndef pgalloc_pmd
static inline pmd_t *pgalloc_pmd(struct memory *mem, uintptr_t addr)
{
    return get_zeroed_page(GFP_KERNEL);
}

static inline void pgfree_pmd(struct memory *mem, pmd_t *pmd)
{
    free_page(pmd);
}
#endif

#ifndef pgalloc_pud
static inline pud_t *pgalloc_pud(struct memory *mem, uintptr_t addr)
{
    return get_zeroed_page(GFP_KERNEL);
}

static inline void pgfree_pud(struct memory *mem, pud_t *pud)
{
    free_page(pud);
}
#endif

#ifndef pgalloc_p4d
static inline p4d_t *pgalloc_p4d(struct memory *mem, uintptr_t addr)
{
    return get_zeroed_page(GFP_KERNEL);
}

static inline void pgfree_p4d(struct memory *mem, p4d_t *p4d)
{
    free_page(p4d);
}
#endif

#ifndef pte_alloc
extern state pte_alloc(struct memory *mem, pmd_t *pmd, uintptr_t addr);
extern void pte_free(struct memory *mem, pmd_t *pmd, uintptr_t addr);
#endif

#ifndef pmd_alloc
extern state pmd_alloc(struct memory *mem, pud_t *pud, uintptr_t addr);
extern void pmd_free(struct memory *mem, pud_t *pud, uintptr_t addr);
#endif

#ifndef pud_alloc
extern state pud_alloc(struct memory *mem, p4d_t *p4d, uintptr_t addr);
extern void pud_free(struct memory *mem, p4d_t *p4d, uintptr_t addr);
#endif

#ifndef p4d_alloc
extern state p4d_alloc(struct memory *mem, pgd_t *pgd, uintptr_t addr);
extern void p4d_free(struct memory *mem, pgd_t *pgd, uintptr_t addr);
#endif

#endif /* _MM_PGTABLE_H_ */
