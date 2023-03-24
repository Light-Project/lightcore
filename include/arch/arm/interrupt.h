/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#ifndef _ARCH_ARM_INTERRUPT_H_
#define _ARCH_ARM_INTERRUPT_H_

#ifndef __ASSEMBLY__

enum exception_vector {
    RST_FAULT       = 0,
    UNI_FAULT       = 1,
    SWI_FAULT       = 2,
    INS_ABORT       = 3,
    DATA_ABORT      = 4,
    IRQ_VECTOR      = 5,
    FIQ_VECTOR      = 6,
};

#endif /* __ASSEMBLY__ */
#endif /* _ARCH_ARM_INTERRUPT_H_ */
