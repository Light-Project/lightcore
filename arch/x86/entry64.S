/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <linkage.h>
#include <asm-offset.h>
#include <asm/segment.h>
#include <asm/irq.h>

/**
 * idtentry - macro to generate entry stubs for idt entries
 * @vector: vector number
 * @asmsym: symbol name for entry point
 * @handle: c function name to called
 * @error_code:
 */
.macro idtentry vector asmsym handle error_code:req
SYM_CODE_START(\asmsym)

SYM_CODE_END(\asmsym)
.endm

/**
 * idtentry_irq - macro to generate entry stubs for irq entries
 * @vector: vector number
 * @asmsym: symbol name for entry point
 * @handle: c function name to called
 */
.macro idtentry_irq vector asmsym handle
SYM_CODE_START(\asmsym)

SYM_CODE_END(\asmsym)
.endm

/**
 * idtentry_nmi - macro to generate entry stubs for nmi entries
 * @vector: vector number
 * @asmsym: symbol name for entry point
 * @handle: c function name to called
 */
.macro idtentry_nmi vector asmsym handle
SYM_CODE_START(\asmsym)

SYM_CODE_END(\asmsym)
.endm

.macro idtentry_mce_db vector asmsym handle
SYM_CODE_START(\asmsym)

SYM_CODE_END(\asmsym)
.endm

/**
 * idtentry_df - macro to generate entry stubs for double fault entries
 * @vector: vector number
 * @asmsym: symbol name for entry point
 * @handle: c function name to called
 */
.macro idtentry_df vector asmsym handle
SYM_CODE_START(\asmsym)

SYM_CODE_END(\asmsym)
.endm

#include <asm/idt.h>

SYM_CODE_START(entry_irq_vector)
    vector = IRQ_EXTERNAL
.rept(IRQ_NR_MAX - IRQ_EXTERNAL)
1:
    /* push vector to error code */
    push    $vector
    jmp     entry_generic_interrupt

    . = 1b + 0x10
    vector = vector + 1
.endr
SYM_CODE_END(entry_irq_vector)
