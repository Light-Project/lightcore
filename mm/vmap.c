/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <mm.h>

#include <asm/pgtable.h>
#include <asm/tlb.h>

static inline pte_t *vmap_pte_get(p4d_t *pgd, size_t addr)
{
    if (unlikely(p4d_none(pgd))) {
        if (unlikely(p4d_alloc(pgd, addr)))
            return NULL;
    }

    return p4d_offset(pgd, addr);
}

static inline pmd_t *vmap_pmd_get(pud_t *pgd, size_t addr)
{
    if (unlikely(pmd_none(pgd))) {
        if (unlikely(pmd_alloc(pgd, addr)))
            return NULL;
    }

    return pmd_offset(pgd, addr);
}

static inline pud_t *vmap_pud_get(p4d_t *pgd, size_t addr)
{
    if (unlikely(pud_none(pgd))) {
        if (unlikely(pud_alloc(pgd, addr)))
            return NULL;
    }

    return pud_offset(pgd, addr);
}

static inline p4d_t *vmap_p4d_get(pgd_t *pgd, size_t addr)
{
    if (unlikely(p4d_none(pgd))) {
        if (unlikely(p4d_alloc(pgd, addr)))
            return NULL;
    }

    return p4d_offset(pgd, addr);
}

static state vmap_pte_range(struct pmd_t *pmd, phys_addr_t phys, size_t addr, size_t size, int flags)
{
    pte_t *pte;
    size_t pfn;
    state ret;

    pte = vmap_pte_get(pmd, addr);
    if (!pte)
        return -ENOMEM;

    pfn = phys >> PAGE_SHIFT;

    for (; size; pfn++, addr += PAGE_SIZE, size -= PAGE_SIZE) {
        pte_set(pte, );
    }

    return -ENOERR;
}

static bool vmap_huge_pmd(struct pmd_t *pmd, phys_addr_t phys, size_t addr, size_t size)
{
    if (!align_check(phys, PMD_SIZE))
        return false;

    if (!align_check(addr, PMD_SIZE))
        return false;

    if (size != PMD_SIZE)
        return false;

    return true;
}

static state vmap_pmd_range(struct pud_t *pud, phys_addr_t phys, size_t addr, size_t size, int flags)
{
    pmd_t *pmd;
    size_t bound;
    state ret;

    pmd = vmap_pmd_get(pud, addr);
    if (!pmd)
        return -ENOMEM;

    for (; size; phys += bound, addr += bound, size -= bound) {
        bound = pmd_bound_size(addr, size);

        if (vmap_huge_pmd(pmd, phys, addr, bound)) {
            pmd_set_huge(pmd, phys);
            continue;
        }

        ret = vmap_pmd_range(pmd, phys, addr, size, flags);
        if (ret)
            return ret;

        pmd++;
    }

    return -ENOERR;
}

static bool vmap_huge_pud(struct pud_t *pud, phys_addr_t phys, size_t addr, size_t size)
{
    if (!align_check(phys, PUD_SIZE))
        return false;

    if (!align_check(addr, PUD_SIZE))
        return false;

    if (size != PMD_SIZE)
        return false;

    return true;
}

static state vmap_pud_range(struct p4d_t *p4d, phys_addr_t phys, size_t addr, size_t size, int flags)
{
    pud_t *pud;
    size_t bound;
    state ret;

    pud = vmap_pud_get(p4d, addr);
    if (!pud)
        return -ENOMEM;

    for (; size; phys += bound, addr += bound, size -= bound) {
        bound = pud_bound_size(addr, size);

        if (vmap_huge_pud(pud, phys, addr, bound)) {
            pud_set_huge(pud, phys);
            continue;
        }

        ret = vmap_pud_range(pud, phys, addr, size, flags);
        if (ret)
            return ret;

        pud++;
    }

    return -ENOERR;
}

static bool vmap_huge_p4d(struct p4d_t *p4d, phys_addr_t phys, size_t addr, size_t size)
{
    if (!align_check(phys, P4D_SIZE))
        return false;

    if (!align_check(addr, P4D_SIZE))
        return false;

    if (size != PMD_SIZE)
        return false;

    return true;
}

static state vmap_p4d_range(struct pgd_t *pgd, phys_addr_t phys, size_t addr, size_t size, int flags)
{
    p4d_t *p4d;
    size_t bound;
    state ret;

    p4d = vmap_pgd_get(pgd, addr);
    if (!p4d)
        return -ENOMEM;

    for (; size; phys += bound, addr += bound, size -= bound) {
        bound = p4d_bound_size(addr, size);

        if (vmap_huge_p4d(p4d, phys, addr, bound)) {
            p4d_set_huge(p4d, phys);
            continue;
        }

        ret = vmap_pud_range(p4d, phys, addr, size, flags);
        if (ret)
            return ret;

        p4d++;
    }

    return -ENOERR;
}

static state vmap_pgd_range(phys_addr_t phys, size_t addr, size_t size)
{
    pgd_t *pgd;

    return -ENOERR;
}

state vmap_page_range(struct memory *mm, phys_addr_t phys, size_t addr, size_t size)
{

}
