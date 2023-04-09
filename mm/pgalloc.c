/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 John Sanpe <sanpeqf@gmail.com>
 */

#include <mm/pgalloc.h>

state pte_alloc(pmd_t *pmd, size_t addr)
{
    pte_t *pte;

    pte = pgalloc_pte();
    if (!pte)
        return -ENOMEM;

    spin_lock(&init_mm.pglock);
    if (likely(!pmd_get_present(pmd))) {
        pmd_populate(pmd, pte);
        pte = NULL;
    }
    spin_unlock(&init_mm.pglock);

    if (pte)
        pgfree_pte(pte);

    return -ENOERR;
}

#ifdef CONFIG_PGTABLE_LEVEL3
state pmd_alloc(pud_t *pud, size_t addr)
{
    pmd_t *pmd;

    pmd = pgalloc_pmd();
    if (!pmd)
        return -ENOMEM;

    spin_lock(&init_mm.pglock);
    if (likely(!pud_get_present(pud))) {
        pud_populate(pud, pmd);
        pmd = NULL;
    }
    spin_unlock(&init_mm.pglock);

    if (pmd)
        pgfree_pmd(pmd);

    return -ENOERR;
}
#endif

#ifdef CONFIG_PGTABLE_LEVEL4
state pud_alloc(p4d_t *p4d, size_t addr)
{
    pud_t *pud;

    pud = pgalloc_pud();
    if (!pud)
        return -ENOMEM;

    spin_lock(&init_mm.pglock);
    if (likely(!pmd_get_present(p4d))) {
        pud_populate(p4d, pud);
        pud = NULL;
    }
    spin_unlock(&init_mm.pglock);

    if (pud)
        pgfree_pud(pud);

    return -ENOERR;
}
#endif

#ifdef CONFIG_PGTABLE_LEVEL5
state p4d_alloc(pgd_t *pgd, size_t addr)
{
    p4d_t *p4d;

    p4d = pgalloc_p4d();
    if (!p4d)
        return -ENOMEM;

    spin_lock(&init_mm.pglock);
    if (likely(!pmd_get_present(pgd))) {
        pud_populate(pgd, p4d);
        p4d = NULL;
    }
    spin_unlock(&init_mm.pglock);

    if (p4d)
        pgfree_p4d(p4d);

    return -ENOERR;
}
#endif
