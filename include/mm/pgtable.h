/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _MM_PGTABLE_H_
#define _MM_PGTABLE_H_

#include <kernel.h>
#include <asm/pgtable.h>

enum pgtbl_modified {
    PGTBL_MODIFIED_PTE  = BIT(0),
    PGTBL_MODIFIED_PMD  = BIT(1),
    PGTBL_MODIFIED_PUD  = BIT(2),
    PGTBL_MODIFIED_P4D  = BIT(3),
    PGTBL_MODIFIED_PGD  = BIT(4),
};

#ifndef pte_index
static inline size_t pte_index(size_t addr)
{
    return (addr >> PAGE_SHIFT) & (PTRS_PER_PTE - 1);
}
#endif

#ifndef pmd_index
static inline size_t pmd_index(size_t addr)
{
    return (addr >> PMD_SHIFT) & (PTRS_PER_PMD - 1);
}
#endif

#ifndef pud_index
static inline size_t pud_index(size_t addr)
{
    return (addr >> PUD_SHIFT) & (PTRS_PER_PUD - 1);
}
#endif

#ifndef p4d_index
static inline size_t p4d_index(size_t addr)
{
    return (addr >> P4D_SHIFT) & (PTRS_PER_P4D - 1);
}
#endif

#ifndef pgd_index
static inline size_t pgd_index(size_t addr)
{
    return (addr >> PGDIR_SHIFT) & (PTRS_PER_PGD - 1);
}
#endif

#ifndef pte_offset
static inline pte_t *pte_offset(pmd_t *pmd, size_t addr)
{
    return (pte_t *)pmd_address(pmd) + pte_index(addr);
}
#endif

#ifndef pmd_offset
static inline pmd_t *pmd_offset(pud_t *pud, size_t addr)
{
    return (pmd_t *)pud_address(pud) + pmd_index(addr);
}
#endif

#ifndef pud_offset
static inline pud_t *pud_offset(p4d_t *p4d, size_t addr)
{
    return (pud_t *)p4d_address(p4d) + pud_index(addr);
}
#endif

#ifndef p4d_offset
static inline p4d_t *p4d_offset(pgd_t *pgd, size_t addr)
{
    return (p4d_t *)pgd_address(pgd) + p4d_index(addr);
}
#endif

#ifndef pgd_offset
static inline pgd_t *pgd_offset(pgd_t *pgd, size_t addr)
{
    return pgd + pgd_index(addr);
}
#endif

#ifndef pmd_bound_size
static inline size_t pmd_bound_size(size_t addr, size_t size)
{
    size_t bound = (addr + PMD_SIZE) & PMD_MASK;
    return min(bound - addr, size);
}
#endif

#ifndef pud_bound_size
static inline size_t pud_bound_size(size_t addr, size_t size)
{
    size_t bound = (addr + PUD_SHIFT) & PUD_MASK;
    return min(bound - addr, size);
}
#endif

#ifndef p4d_bound_size
static inline size_t p4d_bound_size(size_t addr, size_t size)
{
    size_t bound = (addr + P4D_SIZE) & P4D_MASK;
    return min(bound - addr, size);
}
#endif

#ifndef pgd_bound_size
static inline size_t pgd_bound_size(size_t addr, size_t size)
{
    size_t bound = (addr + PGDIR_SIZE) & PGDIR_MASK;
    return min(bound - addr, size);
}
#endif

static inline bool pmd_none_clear_bad(pmd_t *pmd)
{
    if (!pmd_get_present(pmd))
        return true;
    if (unlikely(pmd_inval(pmd))) {
        pmd_clear(pmd);
        return true;
    }
    return false;
}

static inline bool pud_none_clear_bad(pud_t *pud)
{
    if (!pud_get_present(pud))
        return true;
    if (unlikely(pud_inval(pud))) {
        pud_clear(pud);
        return true;
    }
    return false;
}

static inline bool p4d_none_clear_bad(p4d_t *p4d)
{
    if (!p4d_get_present(p4d))
        return true;
    if (unlikely(p4d_inval(p4d))) {
        p4d_clear(p4d);
        return true;
    }
    return false;
}

static inline bool pgd_none_clear_bad(pgd_t *pgd)
{
    if (!pgd_get_present(pgd))
        return true;
    if (unlikely(pgd_inval(pgd))) {
        pgd_clear(pgd);
        return true;
    }
    return false;
}

extern state pte_set(pte_t *pte, phys_addr_t addr, gvm_t flags);
extern state pmd_set_huge(pmd_t *pmd, phys_addr_t addr, gvm_t flags);
extern state pud_set_huge(pud_t *pud, phys_addr_t addr, gvm_t flags);
extern state p4d_set_huge(p4d_t *p4d, phys_addr_t addr, gvm_t flags);
extern state pgd_set_huge(pgd_t *pgd, phys_addr_t addr, gvm_t flags);

#endif  /* _MM_PGTABLE_H_ */
