/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#ifndef _ARCH_CSKY_INTERRUPT_H_
#define _ARCH_CSKY_INTERRUPT_H_

#ifndef __ASSEMBLY__

enum fault_vector {
/*  Mnemonic        Vector     Description          */
    VEC_RESET       = 0,    /* Reset                */
    VEC_ALIGN       = 1,    /* Alignment Check      */
    VEC_ACCESS      = 2,    /* Access Error         */
    VEC_ZERODIV     = 3,    /* Divide Error         */
    VEC_ILLEGAL     = 4,    /* Invalid Opcode       */
    VEC_PRIV        = 5,    /* Privilege Violation  */
    VEC_TRACE       = 6,    /* Debug Exception      */
    VEC_BREAKPOINT  = 7,    /* Breakpoint           */
    VEC_SOFTRESET   = 9,    /* Soft reset           */
    VEC_AUTOVEC     = 10,   /* Soft reset           */
    VEC_FAUTOVEC    = 11,   /* Soft reset           */
    VEC_HWACCEL     = 11,    /* Soft reset           */
    VEC_TLBMISS     = 14,    /* Soft reset           */
    VEC_TRAP0       = 16,
    VEC_TRAP1       = 17,
    VEC_TRAP2       = 18,
    VEC_TRAP3       = 19,
    VEC_TLBMODIFIED = 15,
    VEC_TLBINVALIDL = 20,
    VEC_TLBINVALIDS = 21,
};

#endif  /* __ASSEMBLY__ */

#define IRQ_NR_MAX  128

#endif
