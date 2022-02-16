/* SPDX-License-Identifier: GPL-2.0-or-later */
#include <linkage.h>
#include <asm-offset.h>

SYM_CODE_START(setjmp)
    /* save the preserved register values */
    movq    %rbx, SETJMP_RBX(%rdi)
    movq    %rbp, SETJMP_RBP(%rdi)
    movq    %r12, SETJMP_R12(%rdi)
    movq    %r13, SETJMP_R13(%rdi)
    movq    %r14, SETJMP_R14(%rdi)
    movq    %r15, SETJMP_R15(%rdi)

    /* save the the return address */
    leaq    8(%rsp), %rdx
    movq    %rdx, SETJMP_RSP(%rdi)
    movq    (%rsp), %rdx
    movq    %rdx, SETJMP_RPC(%rdi)

    xorq    %rax, %rax
    ret
SYM_CODE_END(setjmp)

SYM_CODE_START(longjmp)
    movq    SETJMP_RBX(%rdi), %rbx
    movq    SETJMP_RBP(%rdi), %rbp
    movq    SETJMP_R12(%rdi), %r12
    movq    SETJMP_R13(%rdi), %r13
    movq    SETJMP_R14(%rdi), %r14
    movq    SETJMP_R15(%rdi), %r15
    movq    SETJMP_RSP(%rdi), %rsp
    movq    SETJMP_RPC(%rdi), %rdx

	testl   %esi,%esi
	movl    $01,%eax
	cmovel  %eax,%esi
	movl    %esi, %eax

    jmpq    *%rdx
SYM_CODE_END(longjmp)