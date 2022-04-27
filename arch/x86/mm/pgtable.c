/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <mm/vmap.h>

#ifdef CONFIG_PGTABLE_LEVEL5
unsigned int ptrs_per_p4d = 1;
unsigned int pgdir_shift = 39;
#endif

state pte_set(pte_t *pte, phys_addr_t phys, gvm_t flags)
{
    pte->val = PAGE_KERNEL;
    pte->addr = phys >> PAGE_SHIFT;

    if (flags & GVM_NOCACHE)
        pte->pcd = PAGE_PCD_CACHEOFF;

    if (flags & GVM_WCOMBINED)
        pte->pwt = PAGE_PWT_THROUGH;

    return -ENOERR;
}

state pmd_set_huge(pmd_t *pmd, phys_addr_t phys, gvm_t flags)
{
    pmd->val = PAGE_KERNEL_LARGE;
    pmd->addrl = phys >> PMD_SHIFT;

    if (flags & GVM_NOCACHE)
        pmd->pcd = PAGE_PCD_CACHEOFF;

    if (flags & GVM_WCOMBINED)
        pmd->pwt = PAGE_PWT_THROUGH;

    return -ENOERR;
}

state pud_set_huge(pud_t *pud, phys_addr_t phys, gvm_t flags)
{
    pud->val = PAGE_KERNEL_LARGE;
    pud->addrl = phys >> PUD_SHIFT;

    if (flags & GVM_NOCACHE)
        pud->pcd = PAGE_PCD_CACHEOFF;

    if (flags & GVM_WCOMBINED)
        pud->pwt = PAGE_PWT_THROUGH;

    return -ENOERR;
}
