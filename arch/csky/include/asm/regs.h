/* SPDX-License-Identifier: GPL-2.0 */
#ifndef _ASM_REGS_H_
#define _ASM_REGS_H_

#include <types.h>
#include <arch/csky/regs.h>

#ifndef __ASSEMBLY__

#ifdef CONFIG_CSKY_ABIV1
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

#define cprcr(reg) ({       \
    unsigned int tmp;       \
    asm volatile(           \
    "cprcr %0, "reg"\n"     \
    :"=b"(tmp));            \
    tmp;                    \
})

#define cpwcr(reg, val) ({  \
    asm volatile(           \
    "cpwcr %0, "reg"\n"     \
    ::"b"(val));            \
})

#endif  /* CONFIG_CSKY_ABIV1 */

#define mfcr(reg) ({        \
    unsigned int tmp;       \
    asm volatile(           \
    "mfcr %0, "reg"\n"      \
    :"=r"(tmp) ::"memory"); \
    tmp;                    \
})

#define mtcr(reg, val) ({   \
    asm volatile(           \
    "mtcr %0, "reg"\n"      \
    ::"r"(val) : "memory"); \
})

#endif /* __ASSEMBLY__ */
#endif /* __ASM_REGS_OPS_H */
