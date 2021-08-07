/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#ifndef _ARCH_CSKY_INTERRUPT_H_
#define _ARCH_CSKY_INTERRUPT_H_

#ifndef __ASSEMBLY__

enum fault_vector {
/*  Mnemonic        Vector     Description          */
    RS_FAULT        = 0,    /* Reset                */
    AG_FAULT        = 1,    /* Alignment Check      */
    AC_FAULT        = 2,    /* Access Error         */
    DZ_FAULT        = 3,    /* Divide Error         */
    IG_FAULT        = 4,    /* Invalid Opcode       */
    PV_FAULT        = 5,    /* Privilege Violation  */
    DE_FAULT        = 6,    /* Debug Exception      */
    BP_FAULT        = 7,    /* Breakpoint           */
    SR_FAULT        = 9,    /* Soft reset           */
    AUTO_INT        = 10,   /* Soft reset           */
    FAST_INT        = 11,   /* Soft reset           */
};

#endif  /* __ASSEMBLY__ */

#define IRQ_NR_MAX  128

#endif
