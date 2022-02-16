/* SPDX-License-Identifier: GPL-2.0-or-later */
#include <linkage.h>
#include <asm-offset.h>

SYM_CODE_START(setjmp)
    movl    4(%esp), %eax

    /* save the preserved register values */
    movl    %ebx, SETJMP_EBX(%eax)
    movl    %esi, SETJMP_ESI(%eax)
    movl    %edi, SETJMP_EDI(%eax)
    movl    %ebp, SETJMP_EBP(%eax)

    /* save the the return address */
    leal    4(%esp), %ecx
    movl    %esp, SETJMP_ESP(%eax)
    movl    0(%esp), %ecx
    movl    %ecx, SETJMP_EPC(%eax)

    xorl    %eax, %eax
    ret
SYM_CODE_END(setjmp)

SYM_CODE_START(longjmp)
    movl    4(%esp), %ecx
    movl    8(%esp), %eax

    movl    SETJMP_EBX(%ecx), %ebx
    movl    SETJMP_ESI(%ecx), %esi
    movl    SETJMP_EDI(%ecx), %edi
    movl    SETJMP_EBP(%ecx), %ebp
    movl    SETJMP_ESP(%ecx), %esp
    movl    SETJMP_EPC(%ecx), %edx

    jmpl    *%edx
SYM_CODE_END(longjmp)
