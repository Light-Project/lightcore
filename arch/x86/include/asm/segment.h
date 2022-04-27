/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _ASM_X86_SEGMENT_H_
#define _ASM_X86_SEGMENT_H_

#include <arch/x86/segment.h>
#include <arch/x86/tss.h>

#define SEGMENT_RPL_MASK    0x03

/* User mode is privilege level 3: */
#define USER_RPL    GDT_DPL_RING3

#ifndef __ASSEMBLY__

extern void gdte_seg_set(int index, char dpl, char type, size_t base, uint64_t limit);
extern void gdte_sys_set(int index, char type, size_t base, uint64_t limit);
extern void __init arch_gdt_setup(void);

#endif  /* __ASSEMBLY__ */
#endif  /* _ASM_X86_SEGMENT_H_ */
