/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _ASM_GENERIC_PGTABLE_NOPUD_H_
#define _ASM_GENERIC_PGTABLE_NOPUD_H_

#include <asm-generic/pgtable-nop4d.h>

typedef p4d_t pud_t;

#define PUD_SHIFT       P4D_SHIFT
#define PUD_SIZE        (_AC(1,ULL) << PUD_SHIFT)
#define PUD_MASK        (~(PUD_SHIFT - 1))
#define PTRS_PER_PUD    1

#define pud_bound_size pud_bound_size
static inline size_t pud_bound_size(size_t addr, size_t size)
{
    return size;
}

#define p4d_get_present p4d_get_present
static inline bool p4d_get_present(p4d_t *p4d)
{
    return true;
}

#define p4d_inval p4d_inval
static inline bool p4d_inval(p4d_t *p4d)
{
    return false;
}

#define p4d_clear p4d_clear
static inline void p4d_clear(p4d_t *p4d)
{

}

#define pud_offset pud_offset
static inline pud_t *pud_offset(p4d_t *p4d, size_t addr)
{
    return (pud_t *)p4d;
}

#define pgalloc_pud pgalloc_pud
static inline state pgalloc_pud(p4d_t *p4d, size_t addr)
{
    return -ENOERR;
}

#define pgfree_pud pgfree_pud
static inline void pgfree_pud(p4d_t *p4d, size_t addr)
{

}

#define pud_alloc pud_alloc
static inline state pud_alloc(p4d_t *p4d, size_t addr)
{
    return -ENOERR;
}

#endif  /* _ASM_GENERIC_PGTABLE_PUD_H_ */
