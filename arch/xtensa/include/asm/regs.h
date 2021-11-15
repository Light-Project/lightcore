/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _ASM_CSKY_REGS_H_
#define _ASM_CSKY_REGS_H_

#include <types.h>

#ifndef __ASSEMBLY__

struct regs {
    uint32_t pc, ps, sar;
    uint32_t a0, a1, a2, a3;
    uint32_t a4, a5, a6, a7;
    uint32_t a8, a9, a10, a11;
    uint32_t a12, a13, a14, a15;
} __packed;

#endif /* __ASSEMBLY__ */
#endif /* __ASM_REGS_OPS_H */
