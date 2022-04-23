/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _ASM_X86_PAGE_H_
#define _ASM_X86_PAGE_H_

#include <const.h>

#if defined(CONFIG_ARCH_X86_32)

# define PAGE_SHIFT         12
# define PAGE_SIZE          (_AC(1,UL) << PAGE_SHIFT)
# define PAGE_MASK          (~(PAGE_SIZE - 1))

# define PGDIR_SHIFT        22
# define PGDIR_SIZE         (_AC(1,UL) << PGDIR_SHIFT)
# define PGDIR_MASK         (~(PGDIR_SIZE - 1))

# define THREAD_ORDER       1
# define THREAD_SHIFT       (PAGE_SHIFT + THREAD_ORDER)
# define THREAD_SIZE        (_AC(1,UL) << THREAD_SHIFT)
# define THREAD_MASK        (~(THREAD_SIZE - 1))

# define PHYS_SHIFT         32
# define PHYS_SIZE          (_AC(1,ULL) << PHYS_SHIFT)
# define PHYS_MASK          (PHYS_SIZE - 1)

# define VIRTS_SHIFT        32
# define VIRTS_SIZE         (_AC(1,ULL) << VIRTS_SHIFT)
# define VIRTS_MASK         (VIRTS_SIZE - 1)

# define PHYS_PAGE_MASK     (PAGE_MASK & PHYS_MASK)
# define PHYS_PGDIR_MASK    (PGDIR_MASK & PHYS_MASK)

# define PTRS_PER_PTE       1024
# define PTRS_PER_PGD       1024

#elif defined(CONFIG_ARCH_X86_64)

# define PAGE_SHIFT         12
# define PAGE_SIZE          (_AC(1,UL) << PAGE_SHIFT)
# define PAGE_MASK          (~(PAGE_SIZE - 1))

# define PMD_SHIFT          21
# define PMD_SIZE           (_AC(1,UL) << PMD_SHIFT)
# define PMD_MASK           (~(PMD_SIZE - 1))

# define PUD_SHIFT          30
# define PUD_SIZE           (_AC(1,UL) << PUD_SHIFT)
# define PUD_MASK           (~(PUD_SIZE - 1))

#ifndef CONFIG_PGTABLE_LEVEL5
# define PGDIR_SHIFT        39
# define PGDIR_SIZE         (_AC(1,UL) << PGDIR_SHIFT)
# define PGDIR_MASK         (~(PGDIR_SIZE - 1))
#else
# define P4D_SHIFT          39
# define P4D_SIZE           (_AC(1,UL) << P4D_SHIFT)
# define P4D_MASK           (~(P4D_SIZE - 1))

# define PGDIR_SHIFT        pgdir_shift
# define PGDIR_SIZE         (_AC(1,UL) << PGDIR_SHIFT)
# define PGDIR_MASK         (~(PGDIR_SIZE - 1))
#endif

# define THREAD_ORDER       1
# define THREAD_SHIFT       (PAGE_SHIFT + THREAD_ORDER)
# define THREAD_SIZE        (_AC(1,UL) << THREAD_SHIFT)
# define THREAD_MASK        (~(THREAD_SIZE - 1))

# define PHYS_SHIFT         48
# define PHYS_SIZE          (_AC(1,ULL) << PHYS_SHIFT)
# define PHYS_MASK          (PHYS_SIZE - 1)

# define VIRTS_SHIFT        48
# define VIRTS_SIZE         (_AC(1,ULL) << VIRTS_SHIFT)
# define VIRTS_MASK         (VIRTS_SIZE - 1)

# define PHYS_PAGE_MASK     (PAGE_MASK & PHYS_MASK)
# define PHYS_PMD_MASK      (PMD_MASK & PHYS_MASK)
# define PHYS_PUD_MASK      (PUD_MASK & PHYS_MASK)
# define PHYS_P4D_MASK      (P4D_MASK & PHYS_MASK)
# define PHYS_PGDIR_MASK    (PGDIR_MASK & PHYS_MASK)

# define PTRS_PER_PTE       512
# define PTRS_PER_PMD       512
# define PTRS_PER_PUD       512
# define PTRS_PER_P4D       ptrs_per_p4d
# define PTRS_PER_PGD       512

#endif  /* CONFIG_ARCH_X86_64 */

#ifndef __ASSEMBLY__

extern unsigned int pgdir_shift;
extern unsigned int ptrs_per_p4d;

#endif  /* __ASSEMBLY__ */

#include <asm-generic/page.h>

#endif  /* _ASM_X86_PAGE_H_ */
