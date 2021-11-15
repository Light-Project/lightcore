/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <linkage.h>

#define kern_sp ss0
#define user_sp ss1
#define tmp0    ss2
#define tmp1    ss3

.macro SAVE_STACK
    subi    sp, 32
    stm     r8-r15, (sp)
.endm

.macro REC_STACK
    ldm     r8-r15, (sp)
    addi    sp, 32
.endm

.macro SWITCH_TO_KERNEL
    mtcr    sp, user_sp
    mfcr    sp, kern_sp
.endm

.macro SWITCH_TO_USER
    mtcr    sp, kern_sp
    mfcr    sp, user_sp
.endm

.macro SAVE_REGS
    mtcr    r1, tmp1
    mfcr    r1, epsr
    btsti   r1, 31
    bt      1f
    SWITCH_TO_KERNEL
1:
    mtcr    sp, tmp0
    subi    sp, 0x20
    subi    sp, 0x20
    subi    sp, 0x08

    stw     r1, (sp, 0x00)

    mfcr    r1, epc
    stw     r1, (sp, 0x04)

    mfcr    r1, tmp0
    bt      2f
    mfcr	r1, user_sp
2:
    stw     r1, (sp, 0x08)

    addi    sp, 0x0c

    mfcr    r1, tmp1
    stm     r1-r15, (sp)

    subi    sp, 0x0c
.endm

.macro REC_REGS
    ldw     r1, (sp, 0x00)
    mtcr    r1, epsr
    btsti   r1, 31

    ldw     r1, (sp, 0x04)
    mtcr    r1, epc

    bt      1f
    ldw     r1, (sp, 0x08)
    mtcr    r1, user_sp
1:

    addi    sp, 0x0c
    ldm     r1-r15, (sp)

    addi    sp, 0x1c
    addi    sp, 0x20

    bt      2f
    SWITCH_TO_USER
2:
.endm

ENTRY(exception_return)
    psrclr  ie

    REC_REGS
    rte
END(exception_return)

SYM_CODE_START(entry_misaligned)
    SAVE_REGS
    psrset  ee

    mov	    a0, sp
    jsri    trap_misaligned

    br      exception_return
SYM_CODE_END(entry_misaligned)

SYM_CODE_START(entry_buserr)
    SAVE_REGS

    mov     a0, sp
    jsri    trap_buserr

    br      exception_return
SYM_CODE_END(entry_buserr)

SYM_CODE_START(entry_zerodiv)
    SAVE_REGS

    mov     a0, sp
    jsri    trap_zerodiv

    br      exception_return
SYM_CODE_END(entry_zerodiv)

SYM_CODE_START(entry_illinsn)
    SAVE_REGS

    mov     a0, sp
    jsri    trap_illinsn

    br      exception_return
SYM_CODE_END(entry_illinsn)

SYM_CODE_START(entry_priv)
    SAVE_REGS

    mov     a0, sp
    jsri    trap_priv

    br      exception_return
SYM_CODE_END(entry_priv)

SYM_CODE_START(entry_pagefault)
    SAVE_REGS
    psrset  ee

    mov     a0, sp
    jsri    arch_pagefault

    br      exception_return
SYM_CODE_END(entry_pagefault)

SYM_CODE_START(entry_syscall)
    SAVE_REGS
    psrset  ee, ie

    REC_REGS
    rfi
SYM_CODE_END(entry_syscall)

SYM_CODE_START(entry_generic_interrupt)
    SAVE_REGS
    psrset  ee

    mov     a0, sp
    jbsr    generic_interrupt

    br      exception_return
SYM_CODE_END(entry_generic_interrupt)
