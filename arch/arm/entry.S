/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <linkage.h>
#include <asm/page.h>

.macro SAVE_REGS
    stmfd   sp!, {r0 - r12}
.endm

.macro REC_REGS
    ldmfd   sp!, {r0 - r12}
.endm

SYM_CODE_START(proc_thread_switch)
    stmfd   sp!, {v1 - fp, lr}
SYM_CODE_END(proc_thread_switch)

SYM_CODE_START(entry_rst)
    SAVE_REGS

    REC_REGS
    bx      lr
SYM_CODE_END(entry_rst)

SYM_CODE_START(entry_uni)
    SAVE_REGS

    REC_REGS
    bx      lr
SYM_CODE_END(entry_uni)

SYM_CODE_START(entry_swi)
    SAVE_REGS

    REC_REGS
    bx      lr
SYM_CODE_END(entry_swi)

SYM_CODE_START(entry_pabt)
    SAVE_REGS

    REC_REGS
    bx      lr
SYM_CODE_END(entry_pabt)

ENTRY(entry_dabt)
    SAVE_REGS

    REC_REGS
    bx      lr
SYM_CODE_END(entry_dabt)

SYM_CODE_START(entry_irq)
    SAVE_REGS

    REC_REGS
    bx      lr
SYM_CODE_END(entry_irq)

SYM_CODE_START(entry_fiq)

    bx      lr
SYM_CODE_END(entry_fiq)

    .section    .vector, "ax"
GLOBAL(vector_table)
    ldr     pc, =entry_rst
    ldr     pc, =entry_uni
    ldr     pc, =entry_swi
    ldr     pc, =entry_pabt
    ldr     pc, =entry_dabt
    ldr     pc, =entry_irq
    ldr     pc, =entry_fiq
END(vector_table)
