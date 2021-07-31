/* SPDX-License-Identifier: GPL-2.0-or-later */
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

.macro SWITCH_TO_KERNEL_STACK

.endm

.macro SWITCH_TO_PROCESS_STACK

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
 * ebp  user stack
 * 0(%ebp) arg6
 */
ENTRY(entry_syscall)
    clac
    SAVE_REGS

    pushl   %esp
    call    trap_general_protection
    subl    $4, %esp

    REC_REGS
    iret
END(entry_syscall)

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
    pushl   %eax

    call    generic_interrupt
    addl    $4, %esp

    iret
END(entry_generic_interrupt)
