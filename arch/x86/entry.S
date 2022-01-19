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
    pushal
    pushl   %gs
    pushl   %fs
    pushl   %ss
    pushl   %es
    pushl   %ds
.endm

.macro REC_REGS
    popl    %ds
    popl    %es
    popl    %ss
    popl    %fs
    popl    %gs
    popal
.endm

SYM_CODE_START(entry_thread_return)

SYM_CODE_END(entry_thread_return)

SYM_CODE_START(entry_switch_stack)
    /* save stack pointer */
    movl    %esp, %eax

    /* save callee-saved registers */
    pushl   %ebx
    pushl   %esi
    pushl   %edi
    pushl   %ebp

    /* save flags register */
    pushfl

    /* switch stack */
    movl    4(%eax), %ebx
    movl    %esp, %ebx
    movl    8(%eax), %esp

    /* restore flags register */
    popfl

    /* restore callee-saved registers */
    pushl   %ebp
    pushl   %edi
    pushl   %esi
    pushl   %ebx

    ret
SYM_CODE_END(entry_switch_stack)



SYM_CODE_START(entry_divide_error)
    cld
    SAVE_REGS

    pushl   %esp
    call    trap_divide_error
    addl    $4, %esp

    REC_REGS
    iret
SYM_CODE_END(entry_divide_error)

SYM_CODE_START(entry_nmi_interrupt)
    cld
    SAVE_REGS

    pushl   %esp
    call    trap_nmi_interrupt
    addl    $4, %esp

    REC_REGS
    iret
SYM_CODE_END(entry_nmi_interrupt)

SYM_CODE_START(entry_breakpoint)
    cld
    SAVE_REGS

    pushl   %esp
    call    trap_breakpoint
    addl    $4, %esp

    REC_REGS
    iret
SYM_CODE_END(entry_breakpoint)

SYM_CODE_START(entry_bounds)
    cld
    SAVE_REGS

    pushl   %esp
    call    trap_bounds
    addl    $4, %esp

    REC_REGS
    iret
SYM_CODE_END(entry_bounds)

SYM_CODE_START(entry_invalid_opcode)
    cld
    SAVE_REGS

    pushl   %esp
    call    trap_invalid_opcode
    addl    $4, %esp

    REC_REGS
    iret
SYM_CODE_END(entry_invalid_opcode)

SYM_CODE_START(entry_device_not_available)
    cld
    SAVE_REGS

    pushl   %esp
    call    trap_device_not_available
    addl    $4, %esp

    REC_REGS
    iret
SYM_CODE_END(entry_device_not_available)

SYM_CODE_START(entry_invalid_tss)
    cld
    SAVE_REGS

    pushl   %esp
    call    trap_invalid_tss
    addl    $4, %esp

    REC_REGS
    iret
SYM_CODE_END(entry_invalid_tss)

SYM_CODE_START(entry_segment_not_present)
    cld
    SAVE_REGS

    pushl   %esp
    call    trap_segment_not_present
    addl    $4, %esp

    REC_REGS
    iret
SYM_CODE_END(entry_segment_not_present)

SYM_CODE_START(entry_stack_segment)
    cld
    SAVE_REGS

    pushl   %esp
    call    trap_stack_segment
    addl    $4, %esp

    REC_REGS
    iret
SYM_CODE_END(entry_stack_segment)

SYM_CODE_START(entry_spurious_interrupt)
    iret
SYM_CODE_END(entry_spurious_interrupt)

SYM_CODE_START(entry_coprocessor_error)
    cld
    SAVE_REGS

    pushl   %esp
    call    trap_coprocessor_error
    addl    $4, %esp

    REC_REGS
    iret
SYM_CODE_END(entry_coprocessor_error)

SYM_CODE_START(entry_alignment_check)
    cld
    SAVE_REGS

    pushl   %esp
    call    trap_alignment_check
    addl    $4, %esp

    REC_REGS
    iret
SYM_CODE_END(entry_alignment_check)

SYM_CODE_START(entry_simd_error)
    cld
    SAVE_REGS

    pushl   %esp
    call    trap_simd_error
    addl    $4, %esp

    REC_REGS
    iret
SYM_CODE_END(entry_simd_error)

SYM_CODE_START(entry_general_protection)
    cld
    SAVE_REGS

    pushl   %esp
    call    trap_general_protection
    addl    $4, %esp

    REC_REGS
    iret
SYM_CODE_END(entry_general_protection)

SYM_CODE_START(entry_overflow)
    cld
    SAVE_REGS

    pushl   %esp
    call    trap_overflow
    addl    $4, %esp

    REC_REGS
    iret
SYM_CODE_END(entry_overflow)

SYM_CODE_START(entry_debug)
    cld
    SAVE_REGS

    pushl   %esp
    call    trap_debug
    addl    $4, %esp

    REC_REGS
    iret
SYM_CODE_END(entry_debug)

SYM_CODE_START(entry_exception)
    SAVE_REGS


    REC_REGS
    iret
SYM_CODE_END(entry_exception)

SYM_CODE_START(entry_double_fault)
    // clts
    // call    trap_double_fault

loop:
    rep;    hlt
    jmp     loop
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
    cld
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
