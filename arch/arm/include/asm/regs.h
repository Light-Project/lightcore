/* SPDX-License-Identifier: GPL-2.0 */
#ifndef _ASM_ARM_REGS_H_
#define _ASM_ARM_REGS_H_

#include <types.h>

#ifndef __ASSEMBLY__

struct regs {
    uint32_t    tls;
    uint32_t    pc, lr;
    uint32_t    sp, psr;

    uint32_t    a2, a3, a4;
    uint32_t    a5, a6, a7;
    uint32_t    a1, a8, a9;
    uint32_t    a10, a11, a12;
    uint32_t    a13, a14;
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
