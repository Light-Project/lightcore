/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <mm/pgalloc.h>
#include <mm/pgtable.h>
#include <mm/vmap.h>
#include <mm/memmodel.h>
#include <mm/page.h>
#include <asm/cache.h>
#include <asm/tlb.h>

static inline pte_t *vmap_pte_get(pmd_t *pmd, size_t addr)
{
    if (unlikely(!pmd_get_present(pmd)))
        if (unlikely(pte_alloc(pmd, addr)))
            return NULL;

    return pte_offset(pmd, addr);
}

static inline pmd_t *vmap_pmd_get(pud_t *pud, size_t addr)
{
    if (unlikely(!pud_get_present(pud)))
        if (unlikely(pmd_alloc(pud, addr)))
            return NULL;

    return pmd_offset(pud, addr);
}

static inline pud_t *vmap_pud_get(p4d_t *p4d, size_t addr)
{
    if (unlikely(!p4d_get_present(p4d)))
        if (unlikely(pud_alloc(p4d, addr)))
            return NULL;

    return pud_offset(p4d, addr);
}

static inline p4d_t *vmap_p4d_get(pgd_t *pgd, size_t addr)
{
    if (unlikely(!pgd_get_present(pgd)))
        if (unlikely(p4d_alloc(pgd, addr)))
            return NULL;

    return p4d_offset(pgd, addr);
}

#ifdef CONFIG_ARCH_HAS_HUGE_PMD
static bool vmap_huge_pmd(pmd_t *pmd, phys_addr_t phys, size_t addr, size_t size,
                          gvm_t flags, unsigned int pgshift)
{
    if (unlikely(pgshift < PMD_SHIFT))
        return false;

    if (!align_check(phys, PMD_SIZE))
        return false;

    if (!align_check(addr, PMD_SIZE))
        return false;

    if (size != PMD_SIZE)
        return false;

    if (pmd_get_present(pmd))
        return false;

    return pmd_set_huge(pmd, phys, flags);
}
#else /* !CONFIG_ARCH_HAS_HUGE_PMD */
static bool vmap_huge_pmd(pmd_t *pmd, phys_addr_t phys, size_t addr, size_t size,
                          gvm_t flags, unsigned int pgshift)
{
    return false;
}

static bool vunmap_huge_pmd(pmd_t *pmd, size_t addr, size_t size)
{
    return false;
}
#endif /* CONFIG_ARCH_HAS_HUGE_PMD */

#ifdef CONFIG_ARCH_HAS_HUGE_PUD
static bool vmap_huge_pud(pud_t *pud, phys_addr_t phys, size_t addr, size_t size,
                          gvm_t flags, unsigned int pgshift)
{
    if (unlikely(pgshift < PUD_SHIFT))
        return false;

    if (!align_check(phys, PUD_SIZE))
        return false;

    if (!align_check(addr, PUD_SIZE))
        return false;

    if (size != PMD_SIZE)
        return false;

    if (pud_get_present(pud))
        return false;

    return pud_set_huge(pud, phys, flags);
}
#else /* !CONFIG_ARCH_HAS_HUGE_PUD */
static bool vmap_huge_pud(pud_t *pud, phys_addr_t phys, size_t addr, size_t size,
                          gvm_t flags, unsigned int pgshift)
{
    return false;
}

static bool vunmap_huge_pud(pud_t *pud, size_t addr, size_t size)
{
    return false;
}
#endif /* CONFIG_ARCH_HAS_HUGE_PUD */

#ifdef CONFIG_ARCH_HAS_HUGE_P4D
static bool vmap_huge_p4d(p4d_t *p4d, phys_addr_t phys, size_t addr, size_t size,
                          gvm_t flags, unsigned int pgshift)
{
    if (unlikely(pgshift < P4D_SHIFT))
        return false;

    if (!align_check(phys, P4D_SIZE))
        return false;

    if (!align_check(addr, P4D_SIZE))
        return false;

    if (size != PMD_SIZE)
        return false;

    if (p4d_get_present(p4d))
        return false;

    return p4d_set_huge(p4d, phys, flags);
}
#else /* !CONFIG_ARCH_HAS_HUGE_P4D */
static bool vmap_huge_p4d(p4d_t *p4d, phys_addr_t phys, size_t addr, size_t size,
                          gvm_t flags, unsigned int pgshift)
{
    return false;
}

static bool vunmap_huge_p4d(p4d_t *p4d, size_t addr, size_t size)
{
    return false;
}
#endif /* CONFIG_ARCH_HAS_HUGE_P4D */

#ifdef CONFIG_ARCH_HAS_HUGE_PGD
static bool vmap_huge_pgd(pgd_t *pgd, phys_addr_t phys, size_t addr, size_t size,
                          gvm_t flags, unsigned int pgshift)
{
    if (unlikely(pgshift < PGDIR_SHIFT))
        return false;

    if (!align_check(phys, PGDIR_SIZE))
        return false;

    if (!align_check(addr, PGDIR_SIZE))
        return false;

    if (size != PGDIR_SIZE)
        return false;

    if (pgd_get_present(p4d))
        return false;

    return pgd_set_huge(p4d, phys, flags);
}
#else /* !CONFIG_ARCH_HAS_HUGE_PGD */
static bool vmap_huge_pgd(pgd_t *pgd, phys_addr_t phys, size_t addr, size_t size,
                          gvm_t flags, unsigned int pgshift)
{
    return false;
}

static bool vunmap_huge_pgd(pgd_t *pgd, size_t addr, size_t size)
{
    return false;
}
#endif /* CONFIG_ARCH_HAS_HUGE_PGD */

static state vmap_pte_pages(pmd_t *pmd, struct page **pages, unsigned long *index, size_t addr,
                            size_t size, gvm_t flags, enum pgtbl_modified *mod)
{
    struct page *page;
    pte_t *pte;

    pte = vmap_pte_get(pmd, addr);
    if (!pte)
        return -ENOMEM;

    for (; size; (*index)++, addr += PAGE_SIZE, size -= PAGE_SIZE) {
        page = pages[*index];
        if (unlikely(!page))
            return -ENOMEM;

        if (unlikely(pte_get_used(pte)))
            return -EBUSY;

        pte_set(pte, page_to_pa(page), flags);

        ++pte;
    }

    return -ENOERR;
}

static state vmap_pmd_pages(pud_t *pud, struct page **pages, unsigned long *index, size_t addr,
                            size_t size, gvm_t flags, enum pgtbl_modified *mod, unsigned int pgshift)
{
    pmd_t *pmd;
    size_t bound;
    state ret;

    pmd = vmap_pmd_get(pud, addr);
    if (unlikely(!pmd))
        return -ENOMEM;

    for (; size; addr += bound, size -= bound) {
        bound = pmd_bound_size(addr, size);

        ret = vmap_pte_pages(pmd, pages, index, addr, bound, flags, mod);
        if (unlikely(ret))
            return ret;

        ++pmd;
    }

    return -ENOERR;
}

static state vmap_pud_pages(p4d_t *p4d, struct page **pages, unsigned long *index, size_t addr,
                            size_t size, gvm_t flags, enum pgtbl_modified *mod, unsigned int pgshift)
{
    pud_t *pud;
    size_t bound;
    state ret;

    pud = vmap_pud_get(p4d, addr);
    if (unlikely(!pud))
        return -ENOMEM;

    for (; size; addr += bound, size -= bound) {
        bound = pud_bound_size(addr, size);

        ret = vmap_pmd_pages(pud, pages, index, addr, bound, flags, mod, pgshift);
        if (unlikely(ret))
            return ret;

        ++pud;
    }

    return -ENOERR;
}

static state vmap_p4d_pages(pgd_t *pgd, struct page **pages, unsigned long *index, size_t addr,
                            size_t size, gvm_t flags, enum pgtbl_modified *mod, unsigned int pgshift)
{
    p4d_t *p4d;
    size_t bound;
    state ret;

    p4d = vmap_p4d_get(pgd, addr);
    if (unlikely(!p4d))
        return -ENOMEM;

    for (; size; addr += bound, size -= bound) {
        bound = p4d_bound_size(addr, size);

        ret = vmap_pud_pages(p4d, pages, index, addr, bound, flags, mod, pgshift);
        if (unlikely(ret))
            return ret;

        p4d++;
    }

    return -ENOERR;
}

static state vmap_pgd_pages(struct memory *mm, struct page **pages, unsigned long *index, size_t addr,
                            size_t size, gvm_t flags, enum pgtbl_modified *mod, unsigned int pgshift)
{
    pgd_t *pgd;
    size_t bound;
    state ret;

    pgd = pgd_offset(mm->pgdir, addr);

    for (; size; addr += bound, size -= bound) {
        bound = pgd_bound_size(addr, size);

        ret = vmap_p4d_pages(pgd, pages, index, addr, bound, flags, mod, pgshift);
        if (unlikely(ret))
            return ret;

        pgd++;
    }

    return -ENOERR;
}

static state vmap_pte_range(pmd_t *pmd, phys_addr_t phys, size_t addr, size_t size,
                            gvm_t flags, enum pgtbl_modified *mod)
{
    pte_t *pte;

    pte = vmap_pte_get(pmd, addr);
    if (!pte)
        return -ENOMEM;

    for (; size; phys += PAGE_SIZE, addr += PAGE_SIZE, size -= PAGE_SIZE) {
        pte_set(pte, phys, flags);
        pte++;
    }

    return -ENOERR;
}

static state vmap_pmd_range(pud_t *pud, phys_addr_t phys, size_t addr, size_t size,
                            gvm_t flags, enum pgtbl_modified *mod, unsigned int pgshift)
{
    pmd_t *pmd;
    size_t bound;
    state ret;

    pmd = vmap_pmd_get(pud, addr);
    if (unlikely(!pmd))
        return -ENOMEM;

    for (; size; phys += bound, addr += bound, size -= bound) {
        bound = pmd_bound_size(addr, size);

        if (vmap_huge_pmd(pmd, phys, addr, bound, flags, pgshift)) {
            *mod |= PGTBL_MODIFIED_PMD;
            continue;
        }

        ret = vmap_pte_range(pmd, phys, addr, bound, flags, mod);
        if (unlikely(ret))
            return ret;

        ++pmd;
    }

    return -ENOERR;
}

static state vmap_pud_range(p4d_t *p4d, phys_addr_t phys, size_t addr, size_t size,
                            gvm_t flags, enum pgtbl_modified *mod, unsigned int pgshift)
{
    pud_t *pud;
    size_t bound;
    state ret;

    pud = vmap_pud_get(p4d, addr);
    if (unlikely(!pud))
        return -ENOMEM;

    for (; size; phys += bound, addr += bound, size -= bound) {
        bound = pud_bound_size(addr, size);

        if (vmap_huge_pud(pud, phys, addr, bound, flags, pgshift)) {
            *mod |= PGTBL_MODIFIED_PUD;
            continue;
        }

        ret = vmap_pmd_range(pud, phys, addr, bound, flags, mod, pgshift);
        if (unlikely(ret))
            return ret;

        ++pud;
    }

    return -ENOERR;
}

static state vmap_p4d_range(pgd_t *pgd, phys_addr_t phys, size_t addr, size_t size,
                            gvm_t flags, enum pgtbl_modified *mod, unsigned int pgshift)
{
    p4d_t *p4d;
    size_t bound;
    state ret;

    p4d = vmap_p4d_get(pgd, addr);
    if (unlikely(!p4d))
        return -ENOMEM;

    for (; size; phys += bound, addr += bound, size -= bound) {
        bound = p4d_bound_size(addr, size);

        if (vmap_huge_p4d(p4d, phys, addr, bound, flags, pgshift)) {
            *mod |= PGTBL_MODIFIED_P4D;
            continue;
        }

        ret = vmap_pud_range(p4d, phys, addr, bound, flags, mod, pgshift);
        if (unlikely(ret))
            return ret;

        p4d++;
    }

    return -ENOERR;
}

static state vmap_pgd_range(struct memory *mm, phys_addr_t phys, size_t addr, size_t size,
                            gvm_t flags, enum pgtbl_modified *mod, unsigned int pgshift)
{
    pgd_t *pgd;
    size_t bound;
    state ret;

    pgd = pgd_offset(mm->pgdir, addr);

    for (; size; phys += bound, addr += bound, size -= bound) {
        bound = pgd_bound_size(addr, size);

        if (vmap_huge_pgd(pgd, phys, addr, bound, flags, pgshift)) {
            *mod |= PGTBL_MODIFIED_PGD;
            continue;
        }

        ret = vmap_p4d_range(pgd, phys, addr, bound, flags, mod, pgshift);
        if (unlikely(ret))
            return ret;

        pgd++;
    }

    return -ENOERR;
}

static void vunmap_pte_range(pmd_t *pmd, size_t addr, size_t size, enum pgtbl_modified *mod)
{
    pte_t *pte;
    size_t bound;

    pte = pte_offset(pmd, addr);

    for (; size; addr += bound, size -= bound) {
        bound = p4d_bound_size(addr, size);
        ++pte;
    }
}

static void vunmap_pmd_range(pud_t *pud, size_t addr, size_t size, enum pgtbl_modified *mod)
{
    pmd_t *pmd;
    size_t bound;

    pmd = pmd_offset(pud, addr);

    for (; size; addr += bound, size -= bound) {
        bound = pmd_bound_size(addr, size);

        if (vunmap_huge_pmd(pmd, addr, bound))
            continue;

        vunmap_pte_range(pmd, addr, bound, mod);
        ++pmd;
    }
}

static void vunmap_pud_range(p4d_t *p4d, size_t addr, size_t size, enum pgtbl_modified *mod)
{
    pud_t *pud;
    size_t bound;

    pud = pud_offset(p4d, addr);

    for (; size; addr += bound, size -= bound) {
        bound = pud_bound_size(addr, size);

        if (vunmap_huge_pud(pud, addr, bound))
            continue;

        vunmap_pmd_range(pud, addr, bound, mod);
        ++pud;
    }
}

static void vunmap_p4d_range(pgd_t *pgd, size_t addr, size_t size, enum pgtbl_modified *mod)
{
    p4d_t *p4d;
    size_t bound;

    p4d = p4d_offset(pgd, addr);

    for (; size; addr += bound, size -= bound) {
        bound = p4d_bound_size(addr, size);

        if (vunmap_huge_p4d(p4d, addr, bound))
            continue;

        vunmap_pud_range(p4d, addr, bound, mod);
        ++p4d;
    }
}

static void vunmap_pgd_range(struct memory *mm, size_t addr, size_t size, enum pgtbl_modified *mod)
{
    pgd_t *pgd;
    size_t bound;

    pgd = pgd_offset(mm->pgdir, addr);

    for (; size; addr += bound, size -= bound) {
        bound = pgd_bound_size(addr, size);

        if (vunmap_huge_pgd(pgd, addr, bound))
            continue;

        vunmap_p4d_range(pgd, addr, bound, mod);
        ++pgd;
    }
}

state vmap_range(struct memory *mm, phys_addr_t phys, size_t addr,
                 size_t size, gvm_t flags, unsigned int pgshift)
{
    enum pgtbl_modified mod;
    state ret;

    ret = vmap_pgd_range(mm, phys, addr, size, flags, &mod, pgshift);
    cache_flush_vmap();

    return ret;
}

state vmap_pages(struct memory *mm, struct page **pages, size_t addr,
                 size_t size, gvm_t flags, unsigned int pgshift)
{
    enum pgtbl_modified mod;
    phys_addr_t phys, pgsize;
    unsigned long count, nr;
    state ret;

    if (pgshift <= PAGE_SHIFT) {
        unsigned long index = 0;
        ret = vmap_pgd_pages(mm, pages, &index, addr, size, flags, &mod, pgshift);
        if (ret)
            goto error;
    } else {
        pgsize = 1UL << pgshift;
        nr = size >> PAGE_SHIFT;

        for (count = 0; count < nr; ++count) {
            phys = page_to_pa(pages[count]);

            ret = vmap_pgd_range(mm, phys, addr, pgsize, flags, &mod, pgshift);
            if (ret)
                goto error;

            addr += pgsize;
        }
    }

    cache_flush_vmap();
    return -ENOERR;

error:
    vunmap_range(mm, addr, size);
    return ret;
}

void vunmap_range(struct memory *mm, size_t addr, size_t size)
{
    enum pgtbl_modified mod;

    cache_flush_vunmap();
    vunmap_pgd_range(mm, addr, size, &mod);
    tlb_inval_range(addr, size);
}
