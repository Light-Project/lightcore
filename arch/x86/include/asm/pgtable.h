/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _ASM_X86_PGTABLE_H_
#define _ASM_X86_PGTABLE_H_

#include <bits.h>
#include <mm/gvm.h>
#include <asm/page.h>
#include <arch/x86/page.h>

typedef struct pte pte_t;
typedef struct pgd pgd_t;

#if CONFIG_PGTABLE_LEVEL > 2
typedef struct pgd pmd_t;
#else
# include <asm-generic/pgtable-nopmd.h>
#endif

#if CONFIG_PGTABLE_LEVEL > 3
typedef struct pgd pud_t;
#else
# include <asm-generic/pgtable-nopud.h>
#endif

#if CONFIG_PGTABLE_LEVEL > 4
typedef struct pgd p4d_t;
#else
# include <asm-generic/pgtable-nop4d.h>
#endif

#define __PP    ((unsigned long)PAGE_PRESENT)
#define __PW    ((unsigned long)PAGE_RW)
#define __PU    ((unsigned long)PAGE_USER)
#define __PC    ((unsigned long)PAGE_PCD)
#define __PA    ((unsigned long)PAGE_ACCESSED)
#define __PD    ((unsigned long)PAGE_DIRTY)
#define __PE    ((unsigned long)PAGE_PSE)
#define __PG    ((unsigned long)PAGE_GLOBAL)
#define __PN    ((unsigned long)PAGE_NX)

#define PAGE_NONE               (   0 |    0 |    0 |    0 | __PA |    0 |    0 | __PG |    0)
#define PAGE_TABLE              (__PP | __PW |    0 |    0 | __PA | __PD |    0 |    0 | __PN)
#define PAGE_TABLE_EXC          (__PP | __PW |    0 |    0 | __PA | __PD |    0 |    0 |    0)
#define PAGE_KERNEL             (__PP | __PW |    0 |    0 | __PA | __PD |    0 | __PG | __PN)
#define PAGE_KERNEL_EXC         (__PP | __PW |    0 |    0 | __PA | __PD |    0 | __PG |    0)
#define PAGE_KERNEL_RO          (__PP |    0 |    0 |    0 | __PA | __PD |    0 | __PG | __PN)
#define PAGE_KERNEL_ROX         (__PP |    0 |    0 |    0 | __PA | __PD |    0 | __PG |    0)
#define PAGE_KERNEL_NC          (__PP | __PW |    0 | __PC | __PA | __PD |    0 | __PG | __PN)
#define PAGE_KERNEL_LARGE       (__PP | __PW |    0 |    0 | __PA | __PD | __PE | __PG | __PN)
#define PAGE_KERNEL_LARGE_EXC   (__PP | __PW |    0 |    0 | __PA | __PD | __PE | __PG |    0)

extern pgd_t page_dir[PTRS_PER_PGD];

static inline void pte_clear(pte_t *pte)
{
    pte->val = 0;
}

static inline bool pte_get_present(pte_t *pte)
{
    return pte->p || pte->g;
}

static inline bool pte_get_used(pte_t *pte)
{
    return !!pte->val;
}

static inline bool pte_get_dirty(pte_t *pte)
{
    return pte->d;
}

static inline void pte_set_dirty(pte_t *pte, bool dirty)
{
    pte->d = dirty;
}

static inline bool pte_get_accessed(pte_t *pte)
{
    return pte->a;
}

static inline void pte_set_accessed(pte_t *pte, bool accessed)
{
    pte->a = accessed;
}

static inline bool pte_get_wrprotect(pte_t *pte)
{
    return pte->rw;
}

static inline void pte_set_wrprotect(pte_t *pte, bool wrprotect)
{
    pte->rw = wrprotect;
}

#define X86_GENERIC_PGTABLE_OPS(name, type, valtype, child)                         \
static inline valtype name##_generic_pfn_mask(type *pgt)                            \
{                                                                                   \
    if (pgt->ps)                                                                    \
        return PHYS_PGDIR_MASK;                                                     \
    else                                                                            \
        return PHYS_PAGE_MASK;                                                      \
}                                                                                   \
                                                                                    \
static inline valtype name##_generic_flags_mask(type *pgt)                          \
{                                                                                   \
    return ~name##_generic_pfn_mask(pgt);                                           \
}                                                                                   \
                                                                                    \
static inline valtype name##_generic_phys(type *pgt)                                \
{                                                                                   \
    return name##_generic_pfn_mask(pgt) & pgt->val;                                 \
}                                                                                   \
                                                                                    \
static inline valtype name##_generic_flags(type *pgt)                               \
{                                                                                   \
    return name##_generic_flags_mask(pgt) & pgt->val;                               \
}                                                                                   \
                                                                                    \
static inline size_t name##_generic_address(type *pgt)                              \
{                                                                                   \
    phys_addr_t phys;                                                               \
    phys = name##_generic_phys(pgt);                                                \
    return (size_t)pa_to_va(phys);                                                  \
}                                                                                   \
                                                                                    \
static inline void name##_generic_populate(type *pgt, child *cpt)                   \
{                                                                                   \
    pgt->val = PAGE_TABLE | va_to_pa(cpt);                                          \
}                                                                                   \
                                                                                    \
static inline bool name##_generic_inval(type *pgt)                                  \
{                                                                                   \
    return (name##_generic_flags(pgt) & ~__PU) != PAGE_TABLE;                       \
}                                                                                   \
                                                                                    \
static inline void name##_generic_clear(type *pgt)                                  \
{                                                                                   \
    pgt->val = 0;                                                                   \
}                                                                                   \
                                                                                    \
static inline bool name##_generic_get_huge(type *pgt)                               \
{                                                                                   \
    return pgt->ps;                                                                 \
}                                                                                   \
                                                                                    \
static inline bool name##_generic_get_present(type *pgt)                            \
{                                                                                   \
    return pgt->p || pgt->g || pgt->ps;                                             \
}                                                                                   \
                                                                                    \
static inline bool name##_generic_get_dirty(type *pgt)                              \
{                                                                                   \
    return pgt->d;                                                                  \
}                                                                                   \
                                                                                    \
static inline void name##_generic_set_dirty(type *pgt, bool dirty)                  \
{                                                                                   \
    pgt->d = dirty;                                                                 \
}                                                                                   \
                                                                                    \
static inline bool name##_generic_get_accessed(type *pgt)                           \
{                                                                                   \
    return pgt->a;                                                                  \
}                                                                                   \
                                                                                    \
static inline void name##_generic_set_accessed(type *pgt, bool accessed)            \
{                                                                                   \
    pgt->a = accessed;                                                              \
}                                                                                   \
                                                                                    \
static inline bool name##_generic_get_wrprotect(type *pgt)                          \
{                                                                                   \
    return pgt->rw;                                                                 \
}                                                                                   \
                                                                                    \
static inline void name##_generic_set_wrprotect(type *pgt, bool wrprotect)          \
{                                                                                   \
    pgt->rw = wrprotect;                                                            \
}

X86_GENERIC_PGTABLE_OPS(pmd, pmd_t, pmdval_t, pte_t)
#define pmd_pfn_mask        pmd_generic_pfn_mask
#define pmd_flags_mask      pmd_generic_flags_mask
#define pmd_flags           pmd_generic_flags
#define pmd_address         pmd_generic_address
#define pmd_populate        pmd_generic_populate
#define pmd_inval           pmd_generic_inval
#define pmd_clear           pmd_generic_clear
#define pmd_get_huge        pmd_generic_get_huge
#define pmd_get_present     pmd_generic_get_present
#define pmd_get_dirty       pmd_generic_get_dirty
#define pmd_set_dirty       pmd_generic_set_dirty
#define pmd_get_accessed    pmd_generic_get_accessed
#define pmd_set_accessed    pmd_generic_set_accessed
#define pmd_get_wrprotect   pmd_generic_get_wrprotect
#define pmd_set_wrprotect   pmd_generic_set_wrprotect

#if CONFIG_PGTABLE_LEVEL > 2
X86_GENERIC_PGTABLE_OPS(pud, pud_t, pudval_t, pmd_t)
# define pud_pfn_mask       pud_generic_pfn_mask
# define pud_flags_mask     pud_generic_flags_mask
# define pud_flags          pud_generic_flags
# define pud_address        pud_generic_address
# define pud_populate       pud_generic_populate
# define pud_inval          pud_generic_inval
# define pud_clear          pud_generic_clear
# define pud_get_huge       pud_generic_get_huge
# define pud_get_present    pud_generic_get_present
# define pud_get_dirty      pud_generic_get_dirty
# define pud_set_dirty      pud_generic_set_dirty
# define pud_get_accessed   pud_generic_get_accessed
# define pud_set_accessed   pud_generic_set_accessed
# define pud_get_wrprotect  pud_generic_get_wrprotect
# define pud_set_wrprotect  pud_generic_set_wrprotect
#endif

#if CONFIG_PGTABLE_LEVEL > 3
X86_GENERIC_PGTABLE_OPS(p4d, p4d_t, p4dval_t, pud_t)
# define p4d_pfn_mask       p4d_generic_pfn_mask
# define p4d_flags_mask     p4d_generic_flags_mask
# define p4d_flags          p4d_generic_flags
# define p4d_address        p4d_generic_address
# define p4d_populate       p4d_generic_populate
# define p4d_inval          p4d_generic_inval
# define p4d_clear          p4d_generic_clear
# define p4d_get_huge       p4d_generic_get_huge
# define p4d_get_present    p4d_generic_get_present
# define p4d_get_dirty      p4d_generic_get_dirty
# define p4d_set_dirty      p4d_generic_set_dirty
# define p4d_get_accessed   p4d_generic_get_accessed
# define p4d_set_accessed   p4d_generic_set_accessed
# define p4d_get_wrprotect  p4d_generic_get_wrprotect
# define p4d_set_wrprotect  p4d_generic_set_wrprotect
#endif

#if CONFIG_PGTABLE_LEVEL > 4
X86_GENERIC_PGTABLE_OPS(pgd, pgd_t, pgdval_t, p4d_t)
# define pgd_pfn_mask       pgd_generic_pfn_mask
# define pgd_flags_mask     pgd_generic_flags_mask
# define pgd_flags          pgd_generic_flags
# define pgd_address        pgd_generic_address
# define pgd_populate       pgd_generic_populate
# define pgd_inval          pgd_generic_inval
# define pgd_clear          pgd_generic_clear
# define pgd_get_huge       pgd_generic_get_huge
# define pgd_get_present    pgd_generic_get_present
# define pgd_get_dirty      pgd_generic_get_dirty
# define pgd_set_dirty      pgd_generic_set_dirty
# define pgd_get_accessed   pgd_generic_get_accessed
# define pgd_set_accessed   pgd_generic_set_accessed
# define pgd_get_wrprotect  pgd_generic_get_wrprotect
# define pgd_set_wrprotect  pgd_generic_set_wrprotect
#endif

extern void __init arch_page_setup(void);

#endif  /* _ASM_X86_PGTABLE_H_ */
