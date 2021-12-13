/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _MM_PGALLOC_H_
#define _MM_PGALLOC_H_

#include <mm/page.h>
#include <mm/pgtable.h>
#include <asm/pgalloc.h>

#ifndef pgalloc_pte
static inline pte_t *pgalloc_pte(void)
{
    return get_zeroed_page(GFP_KERNEL);
}

static inline void pgfree_pte(pte_t *pte)
{
    free_page(pte);
}
#endif

#ifndef pgalloc_pmd
static inline pmd_t *pgalloc_pmd(void)
{
    return get_zeroed_page(GFP_KERNEL);
}

static inline void pgfree_pmd(pmd_t *pmd)
{
    free_page(pmd);
}
#endif

#ifndef pgalloc_pud
static inline pud_t *pgalloc_pud(void)
{
    return get_zeroed_page(GFP_KERNEL);
}

static inline void pgfree_pud(pud_t *pud)
{
    free_page(pud);
}
#endif

#ifndef pgalloc_p4d
static inline p4d_t *pgalloc_p4d(void)
{
    return get_zeroed_page(GFP_KERNEL);
}

static inline void pgfree_p4d(p4d_t *p4d)
{
    free_page(p4d);
}
#endif

#ifndef pte_alloc
extern state pte_alloc(pmd_t *pmd, size_t addr);
extern void pte_free(pmd_t *pmd, size_t addr);
#endif

#ifndef pmd_alloc
extern state pmd_alloc(pud_t *pud, size_t addr);
extern void pmd_free(pud_t *pud, size_t addr);
#endif

#ifndef pud_alloc
extern state pud_alloc(p4d_t *p4d, size_t addr);
extern void pud_free(p4d_t *p4d, size_t addr);
#endif

#ifndef p4d_alloc
extern state p4d_alloc(pgd_t *pgd, size_t addr);
extern void p4d_free(pgd_t *pgd, size_t addr);
#endif

#endif  /* _MM_PGTABLE_H_ */
