/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _ASM_GENERIC_PGTABLE_H_
#define _ASM_GENERIC_PGTABLE_H_

#include <kernel.h>

static inline size_t pgd_bound_size(size_t addr, size_t size)
{
    size_t bound = (addr + PGDIR_SIZE) & PGDIR_MASK;
    return bound - addr < size ? bound - addr : size;
}

#endif  /* _ASM_GENERIC_PGTABLE_H_ */
