/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 John Sanpe <sanpeqf@gmail.com>
 */

#include <mm/pgalloc.h>
#include <mm/pgtable.h>
#include <mm/vmap.h>
#include <mm/memmodel.h>
#include <mm/page.h>
#include <asm/cache.h>
#include <asm/tlb.h>
#include <crash.h>
#include <export.h>

static __always_inline pte_t *
vmap_pte_get(struct memory *mem, pmd_t *pmd, size_t addr,
             enum pgtbl_modified *mod)
{
    if (unlikely(!pmd_present(pmd))) {
        if (unlikely(pte_alloc(mem, pmd, addr)))
            return NULL;
    }

    return pte_offset(pmd, addr);
}

static __always_inline pmd_t *
vmap_pmd_get(struct memory *mem, pud_t *pud, size_t addr,
             enum pgtbl_modified *mod)
{
    if (unlikely(!pud_present(pud))) {
        if (unlikely(pmd_alloc(mem, pud, addr)))
            return NULL;
    }

    return pmd_offset(pud, addr);
}

static __always_inline pud_t *
vmap_pud_get(struct memory *mem, p4d_t *p4d, size_t addr,
             enum pgtbl_modified *mod)
{
    if (unlikely(!p4d_present(p4d))) {
        if (unlikely(pud_alloc(mem, p4d, addr)))
            return NULL;
    }

    return pud_offset(p4d, addr);
}

static __always_inline p4d_t *
vmap_p4d_get(struct memory *mem, pgd_t *pgd, size_t addr,
             enum pgtbl_modified *mod)
{
    if (unlikely(!pgd_present(pgd))) {
        if (unlikely(p4d_alloc(mem, pgd, addr)))
            return NULL;
    }

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

    if (pmd_present(pmd))
        return false;

    return pmd_set_huge(pmd, phys, flags);
}

static bool vunmap_huge_pmd(pmd_t *pmd)
{
    if (pmd_get_huge(pmd)) {
        pmd_clear(pmd);
        return true;
    }

    return false;
}
#else /* !CONFIG_ARCH_HAS_HUGE_PMD */
static bool vmap_huge_pmd(pmd_t *pmd, phys_addr_t phys, size_t addr, size_t size,
                          gvm_t flags, unsigned int pgshift)
{
    return false;
}

static bool vunmap_huge_pmd(pmd_t *pmd)
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

    if (pud_present(pud))
        return false;

    return pud_set_huge(pud, phys, flags);
}

static bool vunmap_huge_pud(pud_t *pud)
{
    if (pud_get_huge(pud)) {
        pud_clear(pud);
        return true;
    }

    return false;
}
#else /* !CONFIG_ARCH_HAS_HUGE_PUD */
static bool vmap_huge_pud(pud_t *pud, phys_addr_t phys, size_t addr, size_t size,
                          gvm_t flags, unsigned int pgshift)
{
    return false;
}

static bool vunmap_huge_pud(pud_t *pud)
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

    if (p4d_present(p4d))
        return false;

    return p4d_set_huge(p4d, phys, flags);
}

static bool vunmap_huge_p4d(p4d_t *p4d)
{
    if (p4d_get_huge(p4d)) {
        p4d_clear(p4d);
        return true;
    }

    return false;
}
#else /* !CONFIG_ARCH_HAS_HUGE_P4D */
static bool vmap_huge_p4d(p4d_t *p4d, phys_addr_t phys, size_t addr, size_t size,
                          gvm_t flags, unsigned int pgshift)
{
    return false;
}

static bool vunmap_huge_p4d(p4d_t *p4d)
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

    if (pgd_present(pgd))
        return false;

    return pgd_set_huge(pgd, phys, flags);
}

static bool vunmap_huge_pgd(pgd_t *pgd)
{
    if (pgd_get_huge(pgd)) {
        pgd_clear(pgd);
        return true;
    }

    return false;
}
#else /* !CONFIG_ARCH_HAS_HUGE_PGD */
static bool vmap_huge_pgd(pgd_t *pgd, phys_addr_t phys, size_t addr, size_t size,
                          gvm_t flags, unsigned int pgshift)
{
    return false;
}

static bool vunmap_huge_pgd(pgd_t *pgd)
{
    return false;
}
#endif /* CONFIG_ARCH_HAS_HUGE_PGD */

static __always_inline state
vmap_pte_pages(struct memory *mem, pmd_t *pmd, struct page **pages, unsigned long *index,
               size_t addr, size_t size, gvm_t flags, enum pgtbl_modified *mod)
{
    struct page *page;
    pte_t *pte;

    pte = vmap_pte_get(mem, pmd, addr, mod);
    if (unlikely(!pte))
        return -ENOMEM;

    for (; size; (*index)++, addr += PAGE_SIZE, size -= PAGE_SIZE) {
        page = pages[*index];
        if (unlikely(!page))
            return -ENOMEM;

        if (WARN_ON(!pte_none(pte)))
            return -EBUSY;

        pte_set(pte, page_to_pa(page), flags);

        ++pte;
    }

    return -ENOERR;
}

static __always_inline state
vmap_pmd_pages(struct memory *mem, pud_t *pud, struct page **pages, unsigned long *index, size_t addr,
               size_t size, gvm_t flags, enum pgtbl_modified *mod, unsigned int pgshift)
{
    pmd_t *pmd;
    size_t bound;
    state retval;

    pmd = vmap_pmd_get(mem, pud, addr, mod);
    if (unlikely(!pmd))
        return -ENOMEM;

    for (; size; addr += bound, size -= bound) {
        bound = pmd_bound_size(addr, size);

        retval = vmap_pte_pages(mem, pmd, pages, index, addr, bound, flags, mod);
        if (unlikely(retval))
            return retval;

        ++pmd;
    }

    return -ENOERR;
}

static __always_inline state
vmap_pud_pages(struct memory *mem, p4d_t *p4d, struct page **pages, unsigned long *index, size_t addr,
               size_t size, gvm_t flags, enum pgtbl_modified *mod, unsigned int pgshift)
{
    pud_t *pud;
    size_t bound;
    state retval;

    pud = vmap_pud_get(mem, p4d, addr, mod);
    if (unlikely(!pud))
        return -ENOMEM;

    for (; size; addr += bound, size -= bound) {
        bound = pud_bound_size(addr, size);

        retval = vmap_pmd_pages(mem, pud, pages, index, addr, bound, flags, mod, pgshift);
        if (unlikely(retval))
            return retval;

        ++pud;
    }

    return -ENOERR;
}

static __always_inline state
vmap_p4d_pages(struct memory *mem, pgd_t *pgd, struct page **pages, unsigned long *index, size_t addr,
               size_t size, gvm_t flags, enum pgtbl_modified *mod, unsigned int pgshift)
{
    p4d_t *p4d;
    size_t bound;
    state retval;

    p4d = vmap_p4d_get(mem, pgd, addr, mod);
    if (unlikely(!p4d))
        return -ENOMEM;

    for (; size; addr += bound, size -= bound) {
        bound = p4d_bound_size(addr, size);

        retval = vmap_pud_pages(mem, p4d, pages, index, addr, bound, flags, mod, pgshift);
        if (unlikely(retval))
            return retval;

        p4d++;
    }

    return -ENOERR;
}

static __always_inline state
vmap_pgd_pages(struct memory *mem, struct page **pages, unsigned long *index, size_t addr,
               size_t size, gvm_t flags, enum pgtbl_modified *mod, unsigned int pgshift)
{
    pgd_t *pgd;
    size_t bound;
    state retval;

    pgd = pgd_offset(mem->pgdir, addr);

    for (; size; addr += bound, size -= bound) {
        bound = pgd_bound_size(addr, size);

        retval = vmap_p4d_pages(mem, pgd, pages, index, addr, bound, flags, mod, pgshift);
        if (unlikely(retval))
            return retval;

        pgd++;
    }

    return -ENOERR;
}

static __always_inline state
vmap_pte_range(struct memory *mem, pmd_t *pmd, phys_addr_t phys,
               size_t addr, size_t size, gvm_t flags, enum pgtbl_modified *mod)
{
    pte_t *pte;

    pte = vmap_pte_get(mem, pmd, addr, mod);
    if (unlikely(!pte))
        return -ENOMEM;

    for (; size; phys += PAGE_SIZE, addr += PAGE_SIZE, size -= PAGE_SIZE) {
        if (WARN_ON(!pte_none(pte)))
            return -EBUSY;

        pte_set(pte, phys, flags);

        pte++;
    }

    return -ENOERR;
}

static __always_inline state
vmap_pmd_range(struct memory *mem, pud_t *pud, phys_addr_t phys, size_t addr,
               size_t size, gvm_t flags, enum pgtbl_modified *mod, unsigned int pgshift)
{
    pmd_t *pmd;
    size_t bound;
    state retval;

    pmd = vmap_pmd_get(mem, pud, addr, mod);
    if (unlikely(!pmd))
        return -ENOMEM;

    for (; size; phys += bound, addr += bound, size -= bound) {
        bound = pmd_bound_size(addr, size);

        if (vmap_huge_pmd(pmd, phys, addr, bound, flags, pgshift)) {
            *mod |= PGTBL_MODIFIED_PMD;
            continue;
        }

        retval = vmap_pte_range(mem, pmd, phys, addr, bound, flags, mod);
        if (unlikely(retval))
            return retval;

        ++pmd;
    }

    return -ENOERR;
}

static __always_inline state
vmap_pud_range(struct memory *mem, p4d_t *p4d, phys_addr_t phys, size_t addr,
               size_t size, gvm_t flags, enum pgtbl_modified *mod, unsigned int pgshift)
{
    pud_t *pud;
    size_t bound;
    state retval;

    pud = vmap_pud_get(mem, p4d, addr, mod);
    if (unlikely(!pud))
        return -ENOMEM;

    for (; size; phys += bound, addr += bound, size -= bound) {
        bound = pud_bound_size(addr, size);

        if (vmap_huge_pud(pud, phys, addr, bound, flags, pgshift)) {
            *mod |= PGTBL_MODIFIED_PUD;
            continue;
        }

        retval = vmap_pmd_range(mem, pud, phys, addr, bound, flags, mod, pgshift);
        if (unlikely(retval))
            return retval;

        ++pud;
    }

    return -ENOERR;
}

static __always_inline state
vmap_p4d_range(struct memory *mem, pgd_t *pgd, phys_addr_t phys, size_t addr,
               size_t size, gvm_t flags, enum pgtbl_modified *mod, unsigned int pgshift)
{
    p4d_t *p4d;
    size_t bound;
    state retval;

    p4d = vmap_p4d_get(mem, pgd, addr, mod);
    if (unlikely(!p4d))
        return -ENOMEM;

    for (; size; phys += bound, addr += bound, size -= bound) {
        bound = p4d_bound_size(addr, size);

        if (vmap_huge_p4d(p4d, phys, addr, bound, flags, pgshift)) {
            *mod |= PGTBL_MODIFIED_P4D;
            continue;
        }

        retval = vmap_pud_range(mem, p4d, phys, addr, bound, flags, mod, pgshift);
        if (unlikely(retval))
            return retval;

        p4d++;
    }

    return -ENOERR;
}

static __always_inline state
vmap_pgd_range(struct memory *mem, phys_addr_t phys, size_t addr, size_t size,
               gvm_t flags, enum pgtbl_modified *mod, unsigned int pgshift)
{
    pgd_t *pgd;
    size_t bound;
    state retval;

    pgd = pgd_offset(mem->pgdir, addr);

    for (; size; phys += bound, addr += bound, size -= bound) {
        bound = pgd_bound_size(addr, size);

        if (vmap_huge_pgd(pgd, phys, addr, bound, flags, pgshift)) {
            *mod |= PGTBL_MODIFIED_PGD;
            continue;
        }

        retval = vmap_p4d_range(mem, pgd, phys, addr, bound, flags, mod, pgshift);
        if (unlikely(retval))
            return retval;

        pgd++;
    }

    return -ENOERR;
}

static __always_inline void
vunmap_pte_range(struct memory *mem, pmd_t *pmd, size_t addr,
                 size_t size, enum pgtbl_modified *mod)
{
    pte_t *pte;

    pte = pte_offset(pmd, addr);

    for (; size; addr += PAGE_SIZE, size -= PAGE_SIZE) {
        pte_clear(pte);
        pte++;
    }

    *mod |= PGTBL_MODIFIED_PTE;
}

static __always_inline void
vunmap_pmd_range(struct memory *mem, pud_t *pud, size_t addr,
                 size_t size, enum pgtbl_modified *mod)
{
    pmd_t *pmd;
    size_t bound;
    bool cleared;

    pmd = pmd_offset(pud, addr);

    for (; size; addr += bound, size -= bound) {
        bound = pmd_bound_size(addr, size);

        cleared = vunmap_huge_pmd(pmd);
        if (cleared || pmd_inval(pmd))
            *mod |= PGTBL_MODIFIED_PMD;

        if (cleared || pmd_none_clear_bad(pmd))
            continue;

        vunmap_pte_range(mem, pmd, addr, bound, mod);
        ++pmd;
    }
}

static __always_inline void
vunmap_pud_range(struct memory *mem, p4d_t *p4d, size_t addr,
                 size_t size, enum pgtbl_modified *mod)
{
    pud_t *pud;
    size_t bound;
    bool cleared;

    pud = pud_offset(p4d, addr);

    for (; size; addr += bound, size -= bound) {
        bound = pud_bound_size(addr, size);

        cleared = vunmap_huge_pud(pud);
        if (cleared || pud_inval(pud))
            *mod |= PGTBL_MODIFIED_PUD;

        if (cleared || pud_none_clear_bad(pud))
            continue;

        vunmap_pmd_range(mem, pud, addr, bound, mod);
        ++pud;
    }
}

static __always_inline void
vunmap_p4d_range(struct memory *mem, pgd_t *pgd, size_t addr,
                 size_t size, enum pgtbl_modified *mod)
{
    p4d_t *p4d;
    size_t bound;
    bool cleared;

    p4d = p4d_offset(pgd, addr);

    for (; size; addr += bound, size -= bound) {
        bound = p4d_bound_size(addr, size);

        cleared = vunmap_huge_p4d(p4d);
        if (cleared || p4d_inval(p4d))
            *mod |= PGTBL_MODIFIED_P4D;

        if (cleared || p4d_none_clear_bad(p4d))
            continue;

        vunmap_pud_range(mem, p4d, addr, bound, mod);
        ++p4d;
    }
}

static __always_inline void
vunmap_pgd_range(struct memory *mem, size_t addr,
                 size_t size, enum pgtbl_modified *mod)
{
    pgd_t *pgd;
    size_t bound;
    bool cleared;

    pgd = pgd_offset(mem->pgdir, addr);

    for (; size; addr += bound, size -= bound) {
        bound = pgd_bound_size(addr, size);

        cleared = vunmap_huge_pgd(pgd);
        if (cleared || pgd_inval(pgd))
            *mod |= PGTBL_MODIFIED_PGD;

        if (cleared || pgd_none_clear_bad(pgd))
            continue;

        vunmap_p4d_range(mem, pgd, addr, bound, mod);
        ++pgd;
    }
}

state vmap_range(struct memory *mem, phys_addr_t phys, size_t addr,
                 size_t size, gvm_t flags, unsigned int pgshift)
{
    enum pgtbl_modified mod;
    state retval;

    retval = vmap_pgd_range(mem, phys, addr, size, flags, &mod, pgshift);
    cache_flush_vmap();

    return retval;
}
EXPORT_SYMBOL(vmap_range);

state vmap_pages(struct memory *mem, struct page **pages, size_t addr,
                 size_t size, gvm_t flags, unsigned int pgshift)
{
    enum pgtbl_modified mod;
    phys_addr_t phys, pgsize;
    unsigned long count, nr;
    state retval;

    if (pgshift <= PAGE_SHIFT) {
        unsigned long index = 0;
        retval = vmap_pgd_pages(mem, pages, &index, addr, size, flags, &mod, pgshift);
        if (retval)
            goto error;
    } else {
        pgsize = 1UL << pgshift;
        nr = size >> PAGE_SHIFT;

        for (count = 0; count < nr; ++count) {
            phys = page_to_pa(pages[count]);

            retval = vmap_pgd_range(mem, phys, addr, pgsize, flags, &mod, pgshift);
            if (retval)
                goto error;

            addr += pgsize;
        }
    }

    cache_flush_vmap();
    return -ENOERR;

error:
    vunmap_range(mem, addr, size);
    return retval;
}
EXPORT_SYMBOL(vmap_pages);

void vunmap_range(struct memory *mem, size_t addr, size_t size)
{
    enum pgtbl_modified mod;

    cache_flush_vunmap();
    vunmap_pgd_range(mem, addr, size, &mod);
    tlb_inval_range(addr, size);
}
EXPORT_SYMBOL(vunmap_range);
