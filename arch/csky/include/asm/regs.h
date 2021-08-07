/* SPDX-License-Identifier: GPL-2.0 */
#ifndef _ASM_REGS_H_
#define _ASM_REGS_H_

#include <types.h>
#include <arch/csky/regs.h>

#ifndef __ASSEMBLY__

struct regs {
    uint32_t    tls;
    uint32_t    lr;
    uint32_t    pc;
    uint32_t    sr;
    uint32_t    usp;

};

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

#ifdef CONFIG_CSKY_ABIV1

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

#endif /* __ASSEMBLY__ */
#endif /* __ASM_REGS_OPS_H */
