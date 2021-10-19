/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _ASM_X86_GDT_H_
#define _ASM_X86_GDT_H_

#include <arch/x86/segment.h>
#include <arch/x86/tss.h>

#ifndef __ASSEMBLY__

static inline void gdt_set(void *addr)
{
    asm volatile (
        "lgdtl   (%0)"
        ::"Na"(addr)
    );
}

static inline void tss_set(unsigned int seg)
{
    asm volatile (
        "movl   %0, %%ecx\n"
        "ltr    %%cx\n"
        ::"Na"(seg)
    );
}

void gdte_seg_set(int index, char dpl, char type, size_t base, uint64_t limit);
void gdte_sys_set(int index, char type, size_t base, uint64_t limit);
void gdt_setup(void);

#endif	/* __ASSEMBLY__ */
#endif /* _ASM_X86_IDT_H_ */
