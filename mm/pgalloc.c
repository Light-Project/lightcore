/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
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
