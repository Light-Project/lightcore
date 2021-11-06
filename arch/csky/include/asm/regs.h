/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _ASM_CSKY_REGS_H_
#define _ASM_CSKY_REGS_H_

#include <types.h>
#include <arch/csky/regs.h>

#ifndef __ASSEMBLY__

#ifdef CONFIG_CSKY_ABIV1

struct regs {
    uint32_t psr, pc;
    uint32_t sp, r1;
    uint32_t a0, a1, a2;
    uint32_t a3, a4, a5;
    uint32_t l0, l1, l2;
    uint32_t l3, l4, l5;
    uint32_t gb, lr;
} __packed;

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
