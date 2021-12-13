/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _ASM_X86_TLB_H_
#define _ASM_X86_TLB_H_

#include <types.h>
#include <asm/regs.h>

#ifndef __ASSEMBLY__


static inline void tlb_inval_all(void)
{
    cr3_set(cr3_get());
}

extern void tlb_inval_range(size_t addr, size_t size);

#endif /* __ASSEMBLY__ */
#endif /* _ASM_X86_TLB_H_ */
