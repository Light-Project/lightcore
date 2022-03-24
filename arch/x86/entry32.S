/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <linkage.h>
#include <asm-offset.h>
#include <asm/segment.h>
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

.macro SAVE_REGS switch_stacks=0 skip_gs=0
.if \skip_gs == 0
    pushl   $0
.endif

    pushl   %fs
    pushl   %ss
    pushl   %es
    pushl   %ds
    pushal

.if \switch_stacks != 0

.endif
.endm

.macro REC_REGS pop=0
    popal
    popl    %ds
    popl    %es
    popl    %ss
    popl    %fs
    addl    $(4 + \pop), %esp
.endm

SYM_CODE_START(entry_kthread_return)
    pushl   %ebp
    movl    %esp, %ebp

    call    sched_switch_tail
    pushl   12(%ebp)
    call    *8(%ebp)

    leave
    ret
SYM_CODE_END(entry_kthread_return)

SYM_CODE_START(entry_fork_return)

SYM_CODE_END(entry_fork_return)

SYM_CODE_START_LOCAL_NOALIGN(exception_handle)
    SAVE_REGS switch_stacks=1 skip_gs=1
    movl    %esp, %ebp

    /* get handle addrss */
    movl    REGS_GS(%esp), %eax

    /* fixup error code */
    movl    REGS_ERROR(%esp), %ebx
    movl    $-1, REGS_ERROR(%esp)

    /* Call the handle function */
    pushl   %ebx    /* push arg2: vector */
    pushl   %ebp    /* push arg1: regs */
    call    *%eax
    movl    %ebp, %esp

exception_handle_ret:
    movl    REGS_CS(%esp), %ebx
    andl    $SEGMENT_RPL_MASK, %ebx
    cmpl    $USER_RPL, %eax
    jnb     ret_to_user

    REC_REGS 4
    iret

ret_to_user:
SYM_CODE_END(exception_handle)

/**
 * idtentry - macro to generate entry stubs for idt entries
 * @vector: vector number
 * @asmsym: symbol name for entry point
 * @handle: c function name to called
 * @error_code:
 */
.macro idtentry vector asmsym handle error_code:req
SYM_CODE_START(\asmsym)
    cld

.if \error_code == 0
    /* Cushion error code slot */
    pushl   $0
.endif

    /* Push handle address to gs slot */
    push    $\handle
    /* Invoke the common exception entry */
    jmp     exception_handle
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
    SAVE_REGS switch_stacks=1
    movl    %esp, %ebp

    /* Get the vector number */
    movl    REGS_ERROR(%esp), %ebx
    movl    $-1, REGS_ERROR(%esp)

    /* Call the handle function */
    pushl   %ebx    /* push arg2: vector */
    pushl   %ebp    /* push arg1: regs */
    call    \handle

    movl    %ebp, %esp
    jmp     exception_handle_ret
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

/**
 * idtentry_df - macro to generate entry stubs for double fault entries
 * @vector: vector number
 * @asmsym: symbol name for entry point
 * @handle: c function name to called
 */
.macro idtentry_df vector asmsym handle
SYM_CODE_START(\asmsym)
    clts    /* Clear CR0.TS */
    push    $EFLAGS_FIXED
    popfl   /* Clear EFLAGS.NT */

    call    \handle

L(halt):
    rep;    hlt
    jmp     L(halt)
SYM_CODE_END(\asmsym)
.endm

#include <asm/idt.h>

/**
 * entry_syscall_80 - x86_32 int80 system call entry
 * @eax: syscall number
 * @ebx: syscall arg1
 * @ecx: syscall arg2
 * @edx: syscall arg3
 * @esi: syscall arg4
 * @edi: syscall arg5
 * @ebp: syscall arg6
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

SYM_CODE_START(entry_irq_vector)
    vector = IRQ_EXTERNAL
.rept(IRQ_NR_MAX - IRQ_EXTERNAL)
1:
    /* push vector to error code */
    pushl   $vector
    jmp     entry_generic_interrupt

    . = 1b + 0x10
    vector = vector + 1
.endr
SYM_CODE_END(entry_irq_vector)
