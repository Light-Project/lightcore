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
    stw     lr, (sp, 0x08)

    mfcr    r1, epc
    stw     r1, (sp, 0x0c)

    mfcr    r1, tmp0
    bt	    2f
    mfcr	r1, user_sp
2:
    stw     r1, (sp, 0x04)

    addi    sp, 0x10
    mfcr    r1, tmp1

    stw     r1,  (sp, 0x00)
    stw     r2,  (sp, 0x04)
    stw     r3,  (sp, 0x08)
    stw     r4,  (sp, 0x0c)
    stw     r5,  (sp, 0x10)
    stw     r6,  (sp, 0x14)
    stw     r7,  (sp, 0x18)
    stw     r8,  (sp, 0x1c)
    stw     r9,  (sp, 0x20)
    stw     r10, (sp, 0x24)
    stw     r11, (sp, 0x28)
    stw     r12, (sp, 0x2c)
    stw     r13, (sp, 0x30)
    stw     r14, (sp, 0x34)

    subi    sp, 0x10
.endm

.macro REC_REGS
    ldw     r1, (sp, 0x00)
    mtcr    r1, epsr
    btsti   r1, 31

    ldw     lr, (sp, 0x08)
    ldw     r1, (sp, 0x0c)
    mtcr    r1, epc

    bt      1f
    ldw     r1, (sp, 0x04)
    mtcr    r1, user_sp
1:
    addi    sp, 0x10

    ldw     r1,  (sp, 0x00)
    ldw     r2,  (sp, 0x04)
    ldw     r3,  (sp, 0x08)
    ldw     r4,  (sp, 0x0c)
    ldw     r5,  (sp, 0x10)
    ldw     r6,  (sp, 0x14)
    ldw     r7,  (sp, 0x18)
    ldw     r8,  (sp, 0x1c)
    ldw     r9,  (sp, 0x20)
    ldw     r10, (sp, 0x24)
    ldw     r11, (sp, 0x28)
    ldw     r12, (sp, 0x2c)
    ldw     r13, (sp, 0x30)
    ldw     r14, (sp, 0x34)

    addi    sp, 0x20
    addi    sp, 0x18

    bt      2f
    SWITCH_TO_USER
2:
.endm

.macro EXC_RET
    psrclr  ie
    ldw     r1, (sp, 0x00)
    btsti   r1, 31
    bt      1f

1:
    REC_REGS
    rte
.endm

GLOBAL(entry_page_fault)
    SAVE_REGS
    psrset  ee

    mov     a0, sp
    jsri    arch_page_fault

    EXC_RET
END(entry_page_fault)

GLOBAL(entry_syscall)
    SAVE_REGS
    psrset  ee, ie

    REC_REGS
    rfi
END(entry_syscall)

GLOBAL(entry_generic_interrupt)
    SAVE_REGS

    REC_REGS
    rfi
END(entry_generic_interrupt)
