/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <linkage.h>
#include <arch/x86/interrupt.h>

.macro SAVE_REGS
    pushl   %esp
    pushl   %ss
    pushw   %ds
    pushw   %es
    pushw   %fs
    pushw   %gs
    pushal
.endm

.macro REC_REGS
    popal
    popw    %gs
    popw    %fs
    popw    %es
    popw    %ds
    popl    %ss
    popl    %esp
.endm

.macro SWITCH_TO_KERNEL
.endm

.macro SWITCH_TO_USER
.endm

SYM_CODE_START(entry_switch_stack)

SYM_CODE_END(entry_switch_stack)

SYM_CODE_START(entry_divide_error)
    jmp .
    iret
SYM_CODE_END(entry_divide_error)

SYM_CODE_START(entry_nmi)
    jmp .
    iret
SYM_CODE_END(entry_nmi)

SYM_CODE_START(entry_general_protection)
    SAVE_REGS

    pushl   %esp
    call    trap_general_protection
    addl    $4, %esp

    REC_REGS
    iret
SYM_CODE_END(entry_general_protection)

/**
 * Arguments:
 * eax: system call number
 * ebx: arg1
 * ecx: arg2
 * edx: arg3
 * esi: arg4
 * edi: arg5
 * ebp: arg6
 */
SYM_CODE_START(entry_syscall_80)
    clac
    SAVE_REGS

    pushl   %esp
    call    arch_syscall_80
    addl    $4, %esp

    REC_REGS
    iret
SYM_CODE_END(entry_syscall_80)

SYM_CODE_START(entry_generic_interrupt)
    vector = IRQ_EXTERNAL
.rept(IRQ_NR_MAX - IRQ_EXTERNAL)
1:
    SAVE_REGS
    pushl   $vector
    jmp     2f

    . = 1b + 0x20
    vector = vector + 1
.endr

2:
    call    generic_interrupt
    addl    $4, %esp

    REC_REGS
    iret
SYM_CODE_END(entry_generic_interrupt)
