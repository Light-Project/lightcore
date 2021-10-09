/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _ASM_X86_CPUID_H_
#define _ASM_X86_CPUID_H_

#include <types.h>
#include <arch/x86/cpuid.h>

static inline void cpuid(uint32_t *eax, uint32_t *ebx,
				         uint32_t *ecx, uint32_t *edx)
{
    asm volatile (
        "cpuid"
        :"=a" (*eax), "=b" (*ebx),
         "=c" (*ecx), "=d" (*edx)
	    :"0" (*eax), "2" (*ecx)
    );
}

#endif  /* _ASM_X86_CPUID_H_ */
