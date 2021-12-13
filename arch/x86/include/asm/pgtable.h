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
typedef struct pmd pmd_t;
#else
# include <asm-generic/pgtable-nopmd.h>
#endif

#if CONFIG_PGTABLE_LEVEL > 3
typedef struct pud pud_t;
#else
# include <asm-generic/pgtable-nopud.h>
#endif

#if CONFIG_PGTABLE_LEVEL > 4
typedef struct p4d p4d_t;
#else
# include <asm-generic/pgtable-nop4d.h>
#endif

#define __PP PAGE_PRESENT
#define __PW PAGE_RW
#define __PU PAGE_USER
#define __PC PAGE_PCD
#define __PA PAGE_ACCESSED
#define __PD PAGE_DIRTY
#define __PE PAGE_PSE
#define __PG PAGE_GLOBAL
#define __PN PAGE_NX

#define PAGE_NONE               (   0 |    0 |    0 |    0 | __PA |    0 |    0 | __PG |    0)
#define PAGE_TABLE              (__PP | __PW | __PU |    0 | __PA | __PD |    0 |    0 |    0)
#define PAGE_KERNEL             (__PP | __PW |    0 |    0 | __PA | __PD |    0 | __PG | __PN)
#define PAGE_KERNEL_EXC         (__PP | __PW |    0 |    0 | __PA | __PD |    0 | __PG |    0)
#define PAGE_KERNEL_RO          (__PP |    0 |    0 |    0 | __PA | __PD |    0 | __PG | __PN)
#define PAGE_KERNEL_ROX         (__PP |    0 |    0 |    0 | __PA | __PD |    0 | __PG |    0)
#define PAGE_KERNEL_NC          (__PP | __PW |    0 | __PC | __PA | __PD |    0 | __PG | __PN)
#define PAGE_KERNEL_LARGE       (__PP | __PW |    0 |    0 | __PA | __PD | __PE | __PG | __PN)
#define PAGE_KERNEL_LARGE_EXC   (__PP | __PW |    0 |    0 | __PA | __PD | __PE | __PG |    0)

extern pgd_t page_dir[PTRS_PER_PGD];

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

static inline void pmd_populate(pmd_t *pmd, pte_t *pte)
{
    pmd->val = PAGE_TABLE | va_to_pa(pte);
}

static inline size_t pmd_get_addr(pmd_t *pmd)
{
    phys_addr_t phys;

    if (pmd->ps)
        phys = pmd->addrl << PGDIR_SHIFT;
    else
        phys = pmd->addr << PAGE_SHIFT;

    return (size_t)pa_to_va(phys);
}

static inline bool pmd_get_present(pmd_t *pmd)
{
    return pmd->p || pmd->g || pmd->ps;
}

static inline bool pmd_get_dirty(pmd_t *pmd)
{
    return pmd->d;
}

static inline void pmd_set_dirty(pmd_t *pmd, bool dirty)
{
    pmd->d = dirty;
}

static inline bool pmd_get_accessed(pmd_t *pmd)
{
    return pmd->a;
}

static inline void pmd_set_accessed(pmd_t *pmd, bool accessed)
{
    pmd->a = accessed;
}

static inline bool pmd_get_wrprotect(pmd_t *pmd)
{
    return pmd->rw;
}

static inline void pmd_set_wrprotect(pmd_t *pmd, bool wrprotect)
{
    pmd->rw = wrprotect;
}

#if CONFIG_PAGE_LEVEL > 2
static inline bool pud_get_dirty(pud_t *pud)
{
    return pud->d;
}

static inline void pud_set_dirty(pud_t *pud, bool dirty)
{
    pud->d = dirty;
}

static inline bool pud_get_accessed(pud_t *pud)
{
    return pud->a;
}

static inline void pud_set_accessed(pud_t *pud, bool accessed)
{
    pud->a = accessed;
}

static inline bool pud_get_wrprotect(pud_t *pud)
{
    return pud->rw;
}

static inline void pud_set_wrprotect(pud_t *pud, bool wrprotect)
{
    pud->rw = wrprotect;
}
#endif

#if CONFIG_PAGE_LEVEL > 3
static inline bool p4d_get_dirty(p4d_t *p4d)
{
    return p4d->d;
}

static inline void p4d_set_dirty(p4d_t *p4d, bool dirty)
{
    p4d->d = dirty;
}

static inline bool p4d_get_accessed(p4d_t *p4d)
{
    return p4d->a;
}

static inline void p4d_set_accessed(p4d_t *p4d, bool accessed)
{
    p4d->a = accessed;
}

static inline bool p4d_get_wrprotect(p4d_t *p4d)
{
    return p4d->rw;
}

static inline void p4d_set_wrprotect(p4d_t *p4d, bool wrprotect)
{
    p4d->rw = wrprotect;
}
#endif

#if CONFIG_PAGE_LEVEL > 4
static inline bool pgd_get_dirty(pgd_t *pgd)
{
    return pgd->d;
}

static inline void pgd_set_dirty(pgd_t *pgd, bool dirty)
{
    pgd->d = dirty;
}

static inline bool pgd_get_accessed(pgd_t *pgd)
{
    return pgd->a;
}

static inline void pgd_set_accessed(pgd_t *pgd, bool accessed)
{
    pgd->a = accessed;
}

static inline bool pgd_get_wrprotect(pgd_t *pgd)
{
    return pgd->rw;
}

static inline void pgd_set_wrprotect(pgd_t *pgd, bool wrprotect)
{
    pgd->rw = wrprotect;
}

#endif

state arch_page_map(phys_addr_t phys_addr, size_t addr, size_t size);
void arch_page_setup(void);

#endif  /* _ASM_X86_PGTABLE_H_ */
