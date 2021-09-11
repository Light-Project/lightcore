/* SPDX-License-Identifier: GPL-2.0 */
#ifndef _ASM_CSKY_REGS_H_
#define _ASM_CSKY_REGS_H_

#include <types.h>
#include <arch/csky/regs.h>

#ifndef __ASSEMBLY__

#ifdef CONFIG_CSKY_ABIV1

struct regs {
    uint32_t    psr, sp;
    uint32_t    lr, pc;

    uint32_t    r1, r2, r3;
    uint32_t    r4, r5, r6;
    uint32_t    r7, r8, r9;
    uint32_t    r10, r11, r12;
    uint32_t    r13, r14;
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
