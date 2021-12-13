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
    return (pte_t *)pmd_get_addr(pmd) + pte_index(addr);
}
#endif

#ifndef pmd_offset
static inline pmd_t *pmd_offset(pud_t *pud, size_t addr)
{
    return pud_get_addr(pud) + pmd_index(addr);
}
#endif

#ifndef pud_offset
static inline pud_t *pud_offset(p4d_t *p4d, size_t addr)
{
    return p4d_get_addr(p4d) + pud_index(addr);
}
#endif

#ifndef p4d_offset
static inline p4d_t *p4d_offset(pgd_t *pgd, size_t addr)
{
    return pgd_get_addr(pgd) + p4d_index(addr);
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

extern state pte_set(pte_t *pte, phys_addr_t addr, gvm_t flags);
extern state pmd_set_huge(pmd_t *pmd, phys_addr_t addr, gvm_t flags);
extern state pud_set_huge(pud_t *pud, phys_addr_t addr, gvm_t flags);
extern state p4d_set_huge(p4d_t *p4d, phys_addr_t addr, gvm_t flags);
extern state pgd_set_huge(pgd_t *pgd, phys_addr_t addr, gvm_t flags);

#endif  /* _MM_PGTABLE_H_ */
