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

.macro SWITCH_TO_PROCESS

.endm

ENTRY(entry_divide_error)
    jmp .

    iret
END(entry_divide_error)

ENTRY(entry_nmi)
    jmp .

    iret
END(entry_nmi)

ENTRY(entry_general_protection)
    SAVE_REGS

    pushl   %esp
    call    trap_general_protection
    subl    $4, %esp

    REC_REGS
    iret
END(entry_general_protection)

/**
 * Arguments:
 * eax  system call number
 * ebx  arg1
 * ecx  arg2
 * edx  arg3
 * esi  arg4
 * edi  arg5
 * ebp  arg6
 */
ENTRY(entry_syscall_80)
    clac
    SAVE_REGS

    pushl   %esp
    call    arch_syscall_80
    subl    $4, %esp

    REC_REGS
    iret
END(entry_syscall_80)

ENTRY(entry_generic_interrupt)
    vector=IRQ_EXTERNAL

.rept(IRQ_NR_MAX - IRQ_EXTERNAL)
0:
    movl    $vector, %eax
    jmp	    1f

	. = 0b + 0x10
	vector = vector + 1
.endr

1:
    cli
    pushl   %eax

    call    generic_interrupt
    addl    $4, %esp

    sti
    iret
END(entry_generic_interrupt)
