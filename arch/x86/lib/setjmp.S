/* SPDX-License-Identifier: GPL-2.0-or-later */
#include <linkage.h>
#include <asm-offset.h>

#ifdef CONFIG_ARCH_X86_64

SYM_CODE_START(setjmp)
    movq    %rbx, (%rdi)
    movq    %rbp, 8(%rdi)
    movq    %r12, 16(%rdi)
    movq    %r13, 24(%rdi)
    movq    %r14, 32(%rdi)
    movq    %r15, 40(%rdi)
    leaq    8(%rsp), %rdx
    movq    %rdx, 48(%rdi)
    movq    (%rsp), %rdx
    movq    %rdx, 56(%rdi)
    xorq    %rax, %rax
    ret
SYM_CODE_END(setjmp)

SYM_CODE_START(longjmp)
    movq    %rsi, %rax
    testq   %rax, %rax
    jnz     1f
    incq    %rax
1:
    movq    (%rdi), %rbx
    movq    8(%rdi), %rbp
    movq    16(%rdi), %r12
    movq    24(%rdi), %r13
    movq    32(%rdi), %r14
    movq    40(%rdi), %r15
    movq    48(%rdi), %rdx
    movq    %rdx, %rsp
    movq    56(%rdi), %rdx
    jmpq    *%rdx
SYM_CODE_END(longjmp)

#else /* !CONFIG_ARCH_X86_64 */

SYM_CODE_START(setjmp)
    movl    4(%esp), %eax

    movl    %ebx, SETJMP_EBX(%eax)
    movl    %esi, SETJMP_ESI(%eax)
    movl    %edi, SETJMP_EDI(%eax)
    movl    %ebp, SETJMP_EBP(%eax)

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

#endif  /* CONFIG_ARCH_X86_64 */
