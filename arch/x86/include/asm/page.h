/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _ASM_X86_PAGE_H_
#define _ASM_X86_PAGE_H_

#include <const.h>

/* PAGE_SHIFT determines the page size */
#define PAGE_SHIFT      12
#define PAGE_SIZE       (_AC(1,UL) << PAGE_SHIFT)
#define PAGE_MASK       (~(PAGE_SIZE-1))

#define THREAD_SHIFT    (PAGE_SHIFT + 1)
#define THREAD_SIZE     (PAGE_SIZE * 2)
#define THREAD_MASK     (~(THREAD_SIZE - 1))

#define PGDIR_SHIFT     22
#define PTRS_PER_PGD    1024
#define PTRS_PER_PTE	1024

#ifndef __ASSEMBLY__

#endif	/* __ASSEMBLY__ */
#endif /* _ASM_X86_PAGE_H_ */
