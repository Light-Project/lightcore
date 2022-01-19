/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#ifndef _ARCH_CSKY_INTERRUPT_H_
#define _ARCH_CSKY_INTERRUPT_H_

#ifndef __ASSEMBLY__

enum fault_vector {
/*  Mnemonic            Vector     Description                                  */
    VEC_RESET           = 0,    /* Reset exception                              */
    VEC_ALIGN           = 1,    /* 0x04: Misaligned Access                      */
    VEC_ACCESS          = 2,    /* 0x08: Access Error                           */
    VEC_ZERODIV         = 3,    /* 0x0c: Divided by Zero                        */
    VEC_ILLEGAL         = 4,    /* 0x10: Illegal Instruction                    */
    VEC_PRIV            = 5,    /* 0x14: Privilege Violation                    */
    VEC_TRACE           = 6,    /* 0x18: Trace Exception                        */
    VEC_BREAKPOINT      = 7,    /* 0x1c: Breakpoint Exception                   */
    VEC_UNRECOVER       = 8,    /* 0x20: Unrecoverable Error                    */
    VEC_SOFTRESET       = 9,    /* 0x24: Soft Reset                             */
    VEC_AUTOVEC         = 10,   /* 0x28: Normal Autovector Interrupt            */
    VEC_FAUTOVEC        = 11,   /* 0x2c: Fast Interrupt Autovector (AF used)    */
    VEC_HWACCEL         = 12,   /* 0x30: Hardware Accelerator                   */
    VEC_TLBUNRECOVER    = 13,   /* 0x34: TLB Unrecover Error                    */
    VEC_TLBMISS         = 14,   /* 0x38: TLB Miss Error                         */
    VEC_TLBMODIFIED     = 15,   /* 0x3c: TLB Modified Error                     */
    VEC_TRAP0           = 16,   /* 0x40: Trap exception #0                      */
    VEC_TRAP1           = 17,   /* 0x44: Trap exception #1                      */
    VEC_TRAP2           = 18,   /* 0x48: Trap exception #2                      */
    VEC_TRAP3           = 19,   /* 0x4c: Trap exception #3                      */
    VEC_TLBINVALIDL     = 20,   /* 0x50: TLB Invalid Load Error                 */
    VEC_TLBINVALIDS     = 21,   /* 0x54: TLB Invalid Save Error                 */
};

#endif  /* __ASSEMBLY__ */

#define IRQ_NR_MAX  128

#endif
