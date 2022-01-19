/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _ASM_X86_SWAB_H_
#define _ASM_X86_SWAB_H_

#include <lightcore/types.h>

#define arch_swab32 arch_swab32
static __attribute_const__ __u32 arch_swab32(__u32 val)
{
    asm("bswapl %0" : "=r" (val) : "0" (val));
    return val;
}

#define arch_swab64 arch_swab64
static __attribute_const__ __u64 arch_swab64(__u64 val)
{
#ifdef __i386__
    union {
        struct {
            __u32 a;
            __u32 b;
        } s;
        __u64 u;
    } v;

    v.u = val;

    asm volatile (
        "bswapl %0      \n"
        "bswapl %1      \n"
        "xchgl  %0,%1   \n"
        : "=r" (v.s.a), "=r" (v.s.b)
        : "0" (v.s.a), "1" (v.s.b)
    );

    return v.u;
#else
    asm("bswapq %0" : "=r" (val) : "0" (val));
    return val;
#endif
}

#endif /* _ASM_X86_SWAB_H_ */
