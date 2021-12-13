/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <mm/vmap.h>

state pte_set(pte_t *pte, phys_addr_t phys, gvm_t flags)
{
    pte->val = PAGE_KERNEL;
    pte->addr = phys >> PAGE_SHIFT;

    if (flags & GVM_NOCACHE)
        pte->pcd = PAGE_PCD_CACHEOFF;

    return -ENOERR;
}

state pmd_set_huge(pmd_t *pmd, phys_addr_t phys, gvm_t flags)
{
    pmd->val = PAGE_KERNEL_LARGE;
    pmd->addrl = phys >> PMD_SHIFT;

    if (flags & GVM_NOCACHE)
        pmd->pcd = PAGE_PCD_CACHEOFF;

    return -ENOERR;
}
