/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 John Sanpe <sanpeqf@gmail.com>
 */

#include <linkage.h>
#include <asm/page.h>

.macro SAVE_REGS
    stmfd   sp!, {r0 - r12}
.endm

.macro REC_REGS
    ldmfd   sp!, {r0 - r12}
.endm

ENTRY(proc_thread_switch)
    stmfd   sp!, {v1 - fp, lr}
END(proc_thread_switch)

ENTRY(entry_rst)
    SAVE_REGS

    REC_REGS
    bx      lr
END(entry_rst)

ENTRY(entry_uni)
    SAVE_REGS

    REC_REGS
    bx      lr
END(entry_uni)

ENTRY(entry_swi)
    SAVE_REGS

    REC_REGS
    bx      lr
END(entry_swi)

ENTRY(entry_pabt)
    SAVE_REGS

    REC_REGS
    bx      lr
END(entry_pabt)

ENTRY(entry_dabt)
    SAVE_REGS

    REC_REGS
    bx      lr
END(entry_dabt)

ENTRY(entry_irq)
    SAVE_REGS

    REC_REGS
    bx      lr
END(entry_irq)

ENTRY(entry_fiq)

    bx      lr
END(entry_fiq)

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
