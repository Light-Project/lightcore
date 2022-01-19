/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _ASM_X86_FPU_H_
#define _ASM_X86_FPU_H_

#include <types.h>

/*
 * fpu_regs - state of legacy x87 FPU regs
 */
struct fpu_regs {
    uint32_t cwd; /* FPU Control Word */
    uint32_t swd; /* FPU Status Word */
    uint32_t twd; /* FPU Tag Word */
    uint32_t fip; /* FPU IP Offset */
    uint32_t fcs; /* FPU IP Selector */
    uint32_t foo; /* FPU Operand Pointer Offset */
    uint32_t fos; /* FPU Operand Pointer Selector */

    /* 8*10 bytes for each FP-reg = 80 bytes */
    uint32_t st_regs[20];
};

/*
 * fpux_regs - state of SSE/MMX FPU regs
 */
struct fpux_regs {
    uint16_t cwd; /* Control Word */
    uint16_t swd; /* Status Word */
    uint16_t twd; /* Tag Word */
    uint16_t fop; /* Last Instruction Opcode */

    union {
        struct {
            uint64_t rip; /* Instruction Pointer */
            uint64_t rdp; /* Data Pointer */
        };
        struct {
            uint32_t fip; /* FPU IP Offset */
            uint32_t fcs; /* FPU IP Selector */
            uint32_t foo; /* FPU Operand Offset */
            uint32_t fos; /* FPU Operand Selector */
        };
    };

    uint32_t mxcsr; /* MXCSR Register State */
    uint32_t mxcsr_mask; /* MXCSR Mask */

    /* 8*16 bytes for each FP-reg = 128 bytes */
    uint32_t st_regs[32];

    /* 16*16 bytes for each XMM-reg = 256 bytes */
    uint32_t xmm_regs[64];
};

#endif  /* _ASM_X86_FPU_H_ */
