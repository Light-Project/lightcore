/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef _ASM_X86_TLB_H
#define _ASM_X86_TLB_H

#include <asm/regs.h>

#ifndef __ASSEMBLY__

static inline void tlb_flush_all(void)
{
    cr3_set(cr3_get());
}

#endif /* __ASSEMBLY__ */
#endif /* _ASM_X86_TLB_H */
