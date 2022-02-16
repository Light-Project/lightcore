/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _ASM_X86_SEGMENT_H_
#define _ASM_X86_SEGMENT_H_

#include <arch/x86/segment.h>
#include <arch/x86/tss.h>

/* User mode is privilege level 3: */
#define USER_RPL    GDT_DPL_RING3

#ifndef __ASSEMBLY__

static inline void gdt_set(void *addr)
{
    asm volatile (
        "lgdt (%0)\n"
        ::"a"(addr)
    );
}

static inline void tss_set(unsigned int seg)
{
    asm volatile (
        "ltr %0\n"
        ::"c"(seg)
    );
}

extern void gdte_seg_set(int index, char dpl, char type, size_t base, uint64_t limit);
extern void gdte_sys_set(int index, char type, size_t base, uint64_t limit);
extern void __init gdt_setup(void);

#endif	/* __ASSEMBLY__ */
#endif /* _ASM_X86_SEGMENT_H_ */
