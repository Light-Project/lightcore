/* SPDX-License-Identifier: GPL-2.0-or-later */
#include <linkage.h>
#include <asm-offset.h>
#include <asm/bug.h>

SYM_CODE_START(getcontext)
    /* load address of the context data structure */
    movl    4(%esp), %ecx

    /* save the preserved register values */
    movl    %ebx, KCONTEXT_EBX(%ecx)
    movl    %esi, KCONTEXT_ESI(%ecx)
    movl    %edi, KCONTEXT_EDI(%ecx)
    movl    %ebp, KCONTEXT_EBP(%ecx)

    /* save the the return address */
    leal    4(%esp), %edx
    movl    %edx, KCONTEXT_ESP(%ecx)
    movl    (%esp), %edx
    movl    %edx, KCONTEXT_EPC(%ecx)

    ret
SYM_CODE_END(getcontext)

SYM_CODE_START(setcontext)
    /* load address of the context data structure */
    movl    4(%esp), %ecx

    /* load the values of all the preserved registers */
    movl    KCONTEXT_EBX(%ecx), %ebx
    movl    KCONTEXT_ESI(%ecx), %esi
    movl    KCONTEXT_EDI(%ecx), %edi
    movl    KCONTEXT_EBP(%ecx), %ebp
    movl    KCONTEXT_ESP(%ecx), %esp
    movl    KCONTEXT_EPC(%ecx), %edx

    jmpl    *%edx
SYM_CODE_END(setcontext)

SYM_CODE_START(swapcontext)
    /* load address of the context data structure */
    movl    4(%esp), %eax
    movl    8(%esp), %ecx

    /* save the preserved register values */
    movl    %ebx, KCONTEXT_EBX(%eax)
    movl    %esi, KCONTEXT_ESI(%eax)
    movl    %edi, KCONTEXT_EDI(%eax)
    movl    %ebp, KCONTEXT_EBP(%eax)

    /* save the the return address */
    leal    4(%esp), %edx
    movl    %edx, KCONTEXT_ESP(%eax)
    movl    (%esp), %edx
    movl    %edx, KCONTEXT_EPC(%eax)

    /* load the values of all the preserved registers */
    movl    KCONTEXT_EBX(%ecx), %ebx
    movl    KCONTEXT_ESI(%ecx), %esi
    movl    KCONTEXT_EDI(%ecx), %edi
    movl    KCONTEXT_EBP(%ecx), %ebp
    movl    KCONTEXT_ESP(%ecx), %esp
    movl    KCONTEXT_EPC(%ecx), %edx

    jmpl    *%edx
SYM_CODE_END(swapcontext)

SYM_CODE_START(makecontext)
    /* load address of the context data structure */
    movl    4(%esp), %eax
    movl    12(%esp), %ecx

    /*
     * load and save the address of the
     * function we are supposed to run.
     */
    movl    8(%esp), %edx
    movl    %edx, KCONTEXT_EPC(%eax)
    movl    %ecx, KCONTEXT_EBX(%eax)

    /*
     * Compute the address of the stack.
     * The information comes from to stack element.
     */
    movl    KCONTEXT_STA(%eax), %edx
    addl    KCONTEXT_STS(%eax), %edx

    /*
     * make room on the new stack for the
     * parameters and context ptr.
     */
    negl    %ecx
    leal    -4(%edx, %ecx, 4), %edx
    negl    %ecx

    /* align the stack */
    andl    $0xfffffff0, %edx
    subl    $4, %edx

    /* store the future stack pointer */
    movl    %edx, KCONTEXT_ESP(%eax)
    movl    $0, KCONTEXT_EBP(%eax)

    /* store context ptr to the new stack */
    movl    %eax, 4(%edx, %ecx, 4)

    /* copy all the parameters */
    jecxz   2f
1:
    movl	12(%esp, %ecx, 4), %eax
    movl	%eax, (%edx, %ecx, 4)
    decl    %ecx
    jnz     1b
2:

    movl    $L(context_ret), (%edx)
    ret

L(context_ret):
    /* load address of the context data structure */
    movl    (%esp, %ebx, 4), %edx

    /* save return value to the prev context */
    movl    %eax, KCONTEXT_RET(%edx)
    movl    %edx, %eax

    /* load address of next context data structure */
    movl    KCONTEXT_LINK(%edx), %ecx
    testl   %ecx, %ecx
    jz      L(exit)

    pushl   %edx
    call    setcontext

L(exit):
    pushl   KCONTEXT_RET(%edx)
    call    kernel_exit
SYM_CODE_END(makecontext)
