/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _ASM_X86_PAGE_H_
#define _ASM_X86_PAGE_H_

#include <const.h>

/* PAGE_SHIFT determines the page size */
#define PAGE_SHIFT      12
#define PAGE_SIZE       (_AC(1,UL) << PAGE_SHIFT)
#define PAGE_MASK       (~(PAGE_SIZE - 1))

#define THREAD_SHIFT    (PAGE_SHIFT + 1)
#define THREAD_SIZE     (_AC(1,UL) << THREAD_SHIFT)
#define THREAD_MASK     (~(THREAD_SIZE - 1))

#define PHYS_SHIFT      32
#define PHYS_SIZE       (_AC(1,ULL) << PHYS_SHIFT)
#define PHYS_MASK       (~(PHYS_SIZE - 1))

#define VIRTS_SHIFT     32
#define VIRTS_SIZE      (_AC(1,ULL) << VIRTS_SHIFT)
#define VIRTS_MASK      (~(VIRTS_SHIFT - 1))

#define PGDIR_SHIFT     22
#define PGDIR_SIZE      (_AC(1,UL) << PGDIR_SHIFT)
#define PGDIR_MASK      (~(PGDIR_SHIFT - 1))

#define PTRS_PER_PGD    1024
#define PTRS_PER_PTE    1024

#define SPARCE_SHIFT    24

#endif  /* _ASM_X86_PAGE_H_ */
