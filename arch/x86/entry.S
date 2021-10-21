/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <linkage.h>
#include <asm/irq.h>

.macro SWITCH_TO_KERNEL
    pushl   %eax
    movl    %cr3, %eax
    test    $PAGE_SIZE, %eax
    jz      1b
    andl    $(~PAGE_SIZE), %eax
    movl    %eax, %cr3
1:  popl    %eax
.endm

.macro SWITCH_TO_USER
    pushl   %eax
    movl    %cr3, %eax
    orl     $(PAGE_SIZE), %eax
    movl    %eax, %cr3
    popl    %eax
.endm

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

SYM_CODE_START(entry_switch_stack)

SYM_CODE_END(entry_switch_stack)

SYM_CODE_START(entry_divide_error)
    SAVE_REGS

    pushl   %esp
    call    trap_general_protection
    addl    $4, %esp

    REC_REGS
    iret
SYM_CODE_END(entry_divide_error)

SYM_CODE_START(entry_nmi_interrupt)
    SAVE_REGS

    pushl   %esp
    call    trap_general_protection
    addl    $4, %esp

    REC_REGS
    iret
SYM_CODE_END(entry_nmi_interrupt)

SYM_CODE_START(entry_breakpoint)
    SAVE_REGS

    pushl   %esp
    call    trap_general_protection
    addl    $4, %esp

    REC_REGS
    iret
SYM_CODE_END(entry_breakpoint)

SYM_CODE_START(entry_bounds)
    SAVE_REGS

    pushl   %esp
    call    trap_general_protection
    addl    $4, %esp

    REC_REGS
    iret
SYM_CODE_END(entry_bounds)

SYM_CODE_START(entry_invalid_opcode)
    SAVE_REGS

    pushl   %esp
    call    trap_general_protection
    addl    $4, %esp

    REC_REGS
    iret
SYM_CODE_END(entry_invalid_opcode)

SYM_CODE_START(entry_device_not_available)
    SAVE_REGS

    pushl   %esp
    call    trap_general_protection
    addl    $4, %esp

    REC_REGS
    iret
SYM_CODE_END(entry_device_not_available)

SYM_CODE_START(entry_invalid_tss)
    SAVE_REGS

    pushl   %esp
    call    trap_general_protection
    addl    $4, %esp

    REC_REGS
    iret
SYM_CODE_END(entry_invalid_tss)

SYM_CODE_START(entry_segment_not_present)
    SAVE_REGS

    pushl   %esp
    call    trap_general_protection
    addl    $4, %esp

    REC_REGS
    iret
SYM_CODE_END(entry_segment_not_present)

SYM_CODE_START(entry_stack_segment)
    SAVE_REGS

    pushl   %esp
    call    trap_general_protection
    addl    $4, %esp

    REC_REGS
    iret
SYM_CODE_END(entry_stack_segment)

SYM_CODE_START(entry_spurious_interrupt)
    iret
SYM_CODE_END(entry_spurious_interrupt)

SYM_CODE_START(entry_coprocessor_error)
    SAVE_REGS

    pushl   %esp
    call    trap_general_protection
    addl    $4, %esp

    REC_REGS
    iret
SYM_CODE_END(entry_coprocessor_error)

SYM_CODE_START(entry_alignment_check)
    SAVE_REGS

    pushl   %esp
    call    trap_general_protection
    addl    $4, %esp

    REC_REGS
    iret
SYM_CODE_END(entry_alignment_check)

SYM_CODE_START(entry_simd_error)
    SAVE_REGS

    pushl   %esp
    call    trap_general_protection
    addl    $4, %esp

    REC_REGS
    iret
SYM_CODE_END(entry_simd_error)

SYM_CODE_START(entry_general_protection)
    SAVE_REGS

    pushl   %esp
    call    trap_general_protection
    addl    $4, %esp

    REC_REGS
    iret
SYM_CODE_END(entry_general_protection)

SYM_CODE_START(entry_overflow)
    jmp .
    iret
SYM_CODE_END(entry_overflow)

SYM_CODE_START(entry_debug)
    jmp .
    iret
SYM_CODE_END(entry_debug)

SYM_CODE_START(entry_double_fault)
    clts
    call    trap_double_fault

1:
    rep;    hlt
    jmp     1b
SYM_CODE_END(entry_double_fault)

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
