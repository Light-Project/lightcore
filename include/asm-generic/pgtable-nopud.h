/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _ASM_GENERIC_PGTABLE_NOPUD_H_
#define _ASM_GENERIC_PGTABLE_NOPUD_H_

#include <types.h>
#include <state.h>
#include <asm-generic/pgtable-nop4d.h>

typedef p4d_t pud_t;

#define PUD_SHIFT       P4D_SHIFT
#define PUD_SIZE        (_AC(1,ULL) << PUD_SHIFT)
#define PUD_MASK        (~(PUD_SHIFT - 1))
#define PTRS_PER_PUD    1

static inline bool p4d_get_present(p4d_t *pud)
{
    return true;
}

static inline state pud_alloc(p4d_t *p4d, size_t addr)
{
    return -ENOERR;
}

#define pud_offset pud_offset
static inline pud_t *pud_offset(p4d_t *p4d, size_t addr)
{
    return (pud_t *)p4d;
}

#define pud_bound_size pud_bound_size
static inline size_t pud_bound_size(size_t addr, size_t size)
{
    return size;
}

#endif  /* _ASM_GENERIC_PGTABLE_PUD_H_ */
