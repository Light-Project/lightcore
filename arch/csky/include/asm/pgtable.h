/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _ASM_CSKY_PGALLOC_H_
#define _ASM_CSKY_PGALLOC_H_

#include <types.h>
#include <state.h>
#include <arch/csky/page.h>

#define pte_index(va) (((va) >> PAGE_SHIFT) & (PTRS_PER_PTE - 1))
#define pde_index(va) ((va) >> PGDIR_SHIFT)

state arch_page_map(size_t pa, size_t va, size_t size);
void arch_page_setup(void);

#endif  /* _ASM_CSKY_PGALLOC_H_ */
