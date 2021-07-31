/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _ASM_X86_PGTABLE_H_
#define _ASM_X86_PGTABLE_H_

#include <arch/x86/page.h>

#include <asm/page.h>

extern struct pde page_dir[1024];

#define pte_index(va) (((va) >> PAGE_SHIFT) & (PTRS_PER_PTE - 1))
#define pde_index(va) ((va) >> PGDIR_SHIFT)

void arch_page_setup(void);

#endif  /* _ASM_X86_PGTABLE_H_ */
