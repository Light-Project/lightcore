/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _ASM_X86_PGTABLE_H_
#define _ASM_X86_PGTABLE_H_

#include <arch/x86/page.h>
#include <asm/page.h>

typedef struct pte pte_t;
typedef struct pgd pgd_t;

#if CONFIG_PGTABLE_LEVEL > 2
typedef struct pmd pmd_t;
#else
# include <asm-generic/pgtable-pmd.h>
#endif

#if CONFIG_PGTABLE_LEVEL > 3
typedef struct pud pud_t;
#else
# include <asm-generic/pgtable-pud.h>
#endif

#if CONFIG_PGTABLE_LEVEL > 4
typedef struct p4d p4d_t;
#else
# include <asm-generic/pgtable-p4d.h>
#endif

extern struct pgd page_dir[1024];

#define pte_index(va) (((va) >> PAGE_SHIFT) & (PTRS_PER_PTE - 1))
#define pde_index(va) ((va) >> PGDIR_SHIFT)

static inline bool pgtable_pte_get_dirty(pte_t *pte)
{
    return pte->d;
}

static inline void pgtable_pte_set_dirty(pte_t *pte, bool dirty)
{
    pte->d = dirty;
}

static inline bool pgtable_pte_get_accessed(pte_t *pte)
{
    return pte->a;
}

static inline void pgtable_pte_set_accessed(pte_t *pte, bool accessed)
{
    pte->a = accessed;
}

static inline bool pgtable_pte_get_wrprotect(pte_t *pte)
{
    return pte->rw;
}

static inline void pgtable_pte_set_wrprotect(pte_t *pte, bool wrprotect)
{
    pte->rw = wrprotect;
}

#if CONFIG_PAGE_LEVEL > 2
static inline bool pgtable_pmd_get_dirty(pmd_t *pmd)
{
    return pmd->d;
}

static inline void pgtable_pmd_set_dirty(pmd_t *pmd, bool dirty)
{
    pmd->d = dirty;
}

static inline bool pgtable_pmd_get_accessed(pmd_t *pmd)
{
    return pmd->a;
}

static inline void pgtable_pmd_set_accessed(pmd_t *pmd, bool accessed)
{
    pmd->a = accessed;
}

static inline bool pgtable_pmd_get_wrprotect(pmd_t *pmd)
{
    return pmd->rw;
}

static inline void pgtable_pmd_set_wrprotect(pmd_t *pmd, bool wrprotect)
{
    pmd->rw = wrprotect;
}
#endif

#if CONFIG_PAGE_LEVEL > 3
static inline bool pgtable_pud_get_dirty(pud_t *pud)
{
    return pud->d;
}

static inline void pgtable_pud_set_dirty(pud_t *pud, bool dirty)
{
    pud->d = dirty;
}

static inline bool pgtable_pud_get_accessed(pud_t *pud)
{
    return pud->a;
}

static inline void pgtable_pud_set_accessed(pud_t *pud, bool accessed)
{
    pud->a = accessed;
}

static inline bool pgtable_pud_get_wrprotect(pud_t *pud)
{
    return pud->rw;
}

static inline void pgtable_pud_set_wrprotect(pud_t *pud, bool wrprotect)
{
    pud->rw = wrprotect;
}
#endif

#if CONFIG_PAGE_LEVEL > 4
static inline bool pgtable_p4d_get_dirty(p4d_t *p4d)
{
    return p4d->d;
}

static inline void pgtable_p4d_set_dirty(p4d_t *p4d, bool dirty)
{
    p4d->d = dirty;
}

static inline bool pgtable_p4d_get_accessed(p4d_t *p4d)
{
    return p4d->a;
}

static inline void pgtable_p4d_set_accessed(p4d_t *p4d, bool accessed)
{
    p4d->a = accessed;
}

static inline bool pgtable_p4d_get_wrprotect(p4d_t *p4d)
{
    return p4d->rw;
}

static inline void pgtable_p4d_set_wrprotect(p4d_t *p4d, bool wrprotect)
{
    p4d->rw = wrprotect;
}
#endif

static inline bool pgtable_pde_get_dirty(pgd_t *pgd)
{
    return pgd->d;
}

static inline void pgtable_pde_set_dirty(pgd_t *pgd, bool dirty)
{
    pgd->d = dirty;
}

static inline bool pgtable_pde_get_accessed(pgd_t *pgd)
{
    return pgd->a;
}

static inline void pgtable_pde_set_accessed(pgd_t *pgd, bool accessed)
{
    pgd->a = accessed;
}

static inline bool pgtable_pde_get_wrprotect(pgd_t *pgd)
{
    return pgd->rw;
}

static inline void pgtable_pde_set_wrprotect(pgd_t *pgd, bool wrprotect)
{
    pgd->rw = wrprotect;
}

state arch_page_map(phys_addr_t phys_addr, size_t addr, size_t size);
void arch_page_setup(void);

#endif  /* _ASM_X86_PGTABLE_H_ */
