/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _ASM_ARM_REGS_H_
#define _ASM_ARM_REGS_H_

#include <types.h>
#include <arch/arm/regs.h>

#ifndef __ASSEMBLY__

struct regs {
    uint32_t    a1, a2, a3, a4;
    uint32_t    v1, v2, v3;
    uint32_t    v4, v5, v6;
    uint32_t    sl, fp, ip;
    uint32_t    sp, lr, pc;
    uint32_t    cpsr;
};

#define cp15_get(CRn, CRm, OP) ({                       \
    uint32_t val;                                       \
    asm volatile (                                      \
        "mrc    p15, 0, %0," #CRn"," #CRm"," #OP"\n"    \
        :"=r"(val) ::"memory"                           \
    );                                                  \
    val;                                                \
})

#define cp15_set(CRn, CRm, OP, Val) ({                  \
    asm volatile (                                      \
        "mcr    p15, 0, %0," #CRn"," #CRm"," #OP"\n"    \
        ::"r"(Val) :"memory"                            \
    );                                                  \
})

static inline void ttb_set(phys_addr_t pa)
{
    cp15_set(c2, c0, 0, pa);
}

#endif /* __ASSEMBLY__ */
#endif /* __ASM_REGS_OPS_H */
