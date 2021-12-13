/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#ifndef _ARCH_CSKY_INTERRUPT_H_
#define _ARCH_CSKY_INTERRUPT_H_

#ifndef __ASSEMBLY__

enum fault_vector {
/*  Mnemonic            Vector     Description                              */
    VEC_RESET           = 0,    /* Reset exception                          */
    VEC_ALIGN           = 1,    /* Misaligned Access                        */
    VEC_ACCESS          = 2,    /* Access Error                             */
    VEC_ZERODIV         = 3,    /* Divided by Zero                          */
    VEC_ILLEGAL         = 4,    /* Illegal Instruction                      */
    VEC_PRIV            = 5,    /* Privilege Violation                      */
    VEC_TRACE           = 6,    /* Trace Exception                          */
    VEC_BREAKPOINT      = 7,    /* Breakpoint Exception                     */
    VEC_UNRECOVER       = 8,    /* Unrecoverable Error                      */
    VEC_SOFTRESET       = 9,    /* Soft Reset                               */
    VEC_AUTOVEC         = 10,   /* Normal Autovector Interrupt              */
    VEC_FAUTOVEC        = 11,   /* Fast Interrupt Autovector (AF used)      */
    VEC_HWACCEL         = 12,   /* Hardware Accelerator                     */
    VEC_TLBUNRECOVER    = 13,   /* Soft reset                               */
    VEC_TLBMISS         = 14,   /* Soft reset                               */
    VEC_TLBMODIFIED     = 15,   /* Soft reset                               */
    VEC_TRAP0           = 16,   /* Trap exception #0                        */
    VEC_TRAP1           = 17,   /* Trap exception #1                        */
    VEC_TRAP2           = 18,   /* Trap exception #2                        */
    VEC_TRAP3           = 19,   /* Trap exception #3                        */
    VEC_TLBINVALIDL     = 20,
    VEC_TLBINVALIDS     = 21,
};

#endif  /* __ASSEMBLY__ */

#define IRQ_NR_MAX  128

#endif
