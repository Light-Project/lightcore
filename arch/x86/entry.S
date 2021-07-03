/* SPDX-License-Identifier: GPL-2.0-or-later */
#include <linkage.h>
 
.macro SAVE_REGS 
    pushl   %esp
    pushfl
    pushw   %ss
    pushw   %cs
    pushw   %ds
    pushw   %es
    pushw   %fs
    pushw   %gs
    pushal

    movl    0x38(%esp), %edx
    movl    %edx, 0x2c(%esp)
.endm

.macro switch_kernel_stack

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

END(entry_syscall)

ENTRY(entry_undefine)
    iret
END(entry_undefine)
