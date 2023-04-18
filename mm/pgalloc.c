/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 John Sanpe <sanpeqf@gmail.com>
 */

#include <mm/pgalloc.h>

state pte_alloc(struct memory *mem, pmd_t *pmd, size_t addr)
{
    pte_t *pte;

    pte = pgalloc_pte(mem, addr);
    if (!pte)
        return -ENOMEM;

    spin_lock(&mem->pglock);
    if (likely(!pmd_present(pmd))) {
        pmd_populate(pmd, pte);
        pte = NULL;
    }
    spin_unlock(&mem->pglock);

    if (unlikely(pte))
        pgfree_pte(mem, pte);

    return -ENOERR;
}

#ifdef CONFIG_PGTABLE_LEVEL3
state pmd_alloc(struct memory *mem, pud_t *pud, size_t addr)
{
    pmd_t *pmd;

    pmd = pgalloc_pmd(mem, addr);
    if (!pmd)
        return -ENOMEM;

    spin_lock(&mem->pglock);
    if (likely(!pud_present(pud))) {
        pud_populate(pud, pmd);
        pmd = NULL;
    }
    spin_unlock(&mem->pglock);

    if (unlikely(pmd))
        pgfree_pmd(mem, pmd);

    return -ENOERR;
}
#endif /* CONFIG_PGTABLE_LEVEL3 */

#ifdef CONFIG_PGTABLE_LEVEL4
state pud_alloc(struct memory *mem, p4d_t *p4d, size_t addr)
{
    pud_t *pud;

    pud = pgalloc_pud(mem, addr);
    if (!pud)
        return -ENOMEM;

    spin_lock(&mem->pglock);
    if (likely(!pmd_present(p4d))) {
        pud_populate(p4d, pud);
        pud = NULL;
    }
    spin_unlock(&mem->pglock);

    if (unlikely(pud))
        pgfree_pud(mem, pud);

    return -ENOERR;
}
#endif /* CONFIG_PGTABLE_LEVEL4 */

#ifdef CONFIG_PGTABLE_LEVEL5
state p4d_alloc(struct memory *mem, pgd_t *pgd, size_t addr)
{
    p4d_t *p4d;

    p4d = pgalloc_p4d(mem, addr);
    if (!p4d)
        return -ENOMEM;

    spin_lock(&mem->pglock);
    if (likely(!pmd_present(pgd))) {
        pud_populate(pgd, p4d);
        p4d = NULL;
    }
    spin_unlock(&mem->pglock);

    if (unlikely(p4d))
        pgfree_p4d(mem, p4d);

    return -ENOERR;
}
#endif /* CONFIG_PGTABLE_LEVEL5 */
