/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#ifndef _ARCH_CSKY_INTERRUPT_H_
#define _ARCH_CSKY_INTERRUPT_H_

#ifndef __ASSEMBLY__

enum fault_vector {
/*  Mnemonic        Vector     Description                                  */
    VEC_RESET       = 0,    /* Reset exception                              */
    VEC_ALIGN       = 1,    /* Unaligned Memory Access exception            */
    VEC_ACCESS      = 2,    /* Access error exception                       */
    VEC_ZERODIV     = 3,    /* Divide Zero exception                        */
    VEC_ILLEGAL     = 4,    /* Illegal instruction exception                */
    VEC_PRIV        = 5,    /* Instruction Privilege violation exception    */
    VEC_TRACE       = 6,    /* Debug exception                              */
    VEC_BREAKPOINT  = 7,    /* Break-point exception                        */
    VEC_UNRECOVER   = 8,    /* Unrecoverable exception                      */
    VEC_SOFTRESET   = 9,    /* Soft reset           */
    VEC_AUTOVEC     = 10,   /* Soft reset           */
    VEC_FAUTOVEC    = 11,   /* Soft reset           */
    VEC_HWACCEL     = 12,   /* Soft reset           */
    VEC_TLBMISS     = 14,   /* Soft reset           */
    VEC_TLBMODIFIED = 15,   /* Soft reset           */
    VEC_TRAP0       = 16,   /* Trap exception #0                            */
    VEC_TRAP1       = 17,   /* Trap exception #1                            */
    VEC_TRAP2       = 18,   /* Trap exception #2                            */
    VEC_TRAP3       = 19,   /* Trap exception #3                            */
    VEC_TLBINVALIDL = 20,
    VEC_TLBINVALIDS = 21,
};

#endif  /* __ASSEMBLY__ */

#define IRQ_NR_MAX  128

#endif
