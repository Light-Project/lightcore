/* SPDX-License-Identifier: GPL-2.0-or-later */
#include <linkage.h>
#include <asm-offset.h>
#include <asm/bug.h>

SYM_CODE_START(getcontext)
    /* save the preserved register values */
    movq    %rbx, KCONTEXT_RBX(%rdi)
    movq    %rbp, KCONTEXT_RBP(%rdi)
    movq    %r12, KCONTEXT_R12(%rdi)
    movq    %r13, KCONTEXT_R13(%rdi)
    movq    %r14, KCONTEXT_R14(%rdi)
    movq    %r15, KCONTEXT_R15(%rdi)

    movq    %r8, KCONTEXT_R8(%rdi)
    movq    %r9, KCONTEXT_R9(%rdi)
    movq    %rcx, KCONTEXT_RCX(%rdi)
    movq    %rdx, KCONTEXT_RDX(%rdi)
    movq    %rsi, KCONTEXT_RSI(%rdi)
    movq    %rdi, KCONTEXT_RDI(%rdi)

    /* save the the return address */
    leaq    8(%rsp), %rdx
    movq    %rdx, KCONTEXT_RSP(%rdi)
    movq    (%rsp), %rdx
    movq    %rdx, KCONTEXT_RPC(%rdi)

    ret
SYM_CODE_END(getcontext)

SYM_CODE_START(setcontext)
    /* load the values of all the preserved registers */
    movq    KCONTEXT_RBX(%rdi), %rbx
    movq    KCONTEXT_RBP(%rdi), %rbp
    movq    KCONTEXT_R12(%rdi), %r12
    movq    KCONTEXT_R13(%rdi), %r13
    movq    KCONTEXT_R14(%rdi), %r14
    movq    KCONTEXT_R15(%rdi), %r15
    movq    KCONTEXT_RSP(%rdi), %rsp
    movq    KCONTEXT_RPC(%rdi), %rdx

    movq    KCONTEXT_R8(%rdi), %r8
    movq    KCONTEXT_R9(%rdi), %r9
    movq    KCONTEXT_RCX(%rdi), %rcx
    movq    KCONTEXT_RDX(%rdi), %rdx
    movq    KCONTEXT_RSI(%rdi), %rsi
    movq    KCONTEXT_RDI(%rdi), %rdi

    jmpq    *%rdx
SYM_CODE_END(setcontext)

SYM_CODE_START(swapcontext)
    /* save the preserved register values */
    movq    %rbx, KCONTEXT_RBX(%rdi)
    movq    %rbp, KCONTEXT_RBP(%rdi)
    movq    %r12, KCONTEXT_R12(%rdi)
    movq    %r13, KCONTEXT_R13(%rdi)
    movq    %r14, KCONTEXT_R14(%rdi)
    movq    %r15, KCONTEXT_R15(%rdi)

    movq    %r8, KCONTEXT_R8(%rdi)
    movq    %r9, KCONTEXT_R9(%rdi)
    movq    %rcx, KCONTEXT_RCX(%rdi)
    movq    %rdx, KCONTEXT_RDX(%rdi)
    movq    %rsi, KCONTEXT_RSI(%rdi)
    movq    %rdi, KCONTEXT_RDI(%rdi)

    /* save the the return address */
    leaq    8(%rsp), %rdx
    movq    %rdx, KCONTEXT_RSP(%rdi)
    movq    (%rsp), %rdx
    movq    %rdx, KCONTEXT_RPC(%rdi)

    /* load the values of all the preserved registers */
    movq    KCONTEXT_RBX(%rdi), %rbx
    movq    KCONTEXT_RBP(%rdi), %rbp
    movq    KCONTEXT_R12(%rdi), %r12
    movq    KCONTEXT_R13(%rdi), %r13
    movq    KCONTEXT_R14(%rdi), %r14
    movq    KCONTEXT_R15(%rdi), %r15
    movq    KCONTEXT_RSP(%rdi), %rsp
    movq    KCONTEXT_RPC(%rdi), %rdx

    movq    KCONTEXT_R8(%rdi), %r8
    movq    KCONTEXT_R9(%rdi), %r9
    movq    KCONTEXT_RCX(%rdi), %rcx
    movq    KCONTEXT_RDX(%rdi), %rdx
    movq    KCONTEXT_RSI(%rdi), %rsi
    movq    KCONTEXT_RDI(%rdi), %rdi

    jmpq    *%rdx
SYM_CODE_END(swapcontext)

SYM_CODE_START(makecontext)
    /*
     * load and save the address of the
     * function we are supposed to run.
     */
    movq    %rsi, KCONTEXT_RPC(%rdi)
    movq    %rdx, KCONTEXT_RBX(%rdi)

    /*
     * Compute the address of the stack.
     * The information comes from to stack element.
     */
    movq    KCONTEXT_STA(%rdi), %rsi
    addq    KCONTEXT_STS(%rdi), %rsi

    /*
     * make room on the new stack for the
     * parameters and context ptr.
     */
    negq    %rcx
    leaq    -8(%rsi, %rcx, 8), %rsi
    negq    %rcx

    /* align the stack */
    andq    $0xfffffffffffffff0, %rsi
    subq    $4, %rsi

    /* store the future stack pointer */
    movq    %rsi, KCONTEXT_RSP(%rdi)
    movq    $0, KCONTEXT_RBP(%rdi)

    /* store context ptr to the new stack */
    movq    %rdi, 8(%rsi, %rdx, 8)

    /* copy all the parameters */
    testq   %rdx, %rdx
    je      2f

    movq    (8 * 1)(%rsp), %rax
    movq    %rax, KCONTEXT_RDI(%rdi)
    decq    %rdx
    jz      2f

    movq    (8 * 2)(%rsp), %rax
    movq    %rax, KCONTEXT_RSI(%rdi)
    decq    %rdx
    jz      2f

    movq    (8 * 3)(%rsp), %rax
    movq    %rax, KCONTEXT_RDX(%rdi)
    decq    %rdx
    jz      2f

    movq    (8 * 4)(%rsp), %rax
    movq    %rax, KCONTEXT_RCX(%rdi)
    decq    %rdx
    jz      2f

    movq    (8 * 5)(%rsp), %rax
    movq    %rax, KCONTEXT_R8(%rdi)
    decq    %rdx
    jz      2f

    movq    (8 * 6)(%rsp), %rax
    movq    %rax, KCONTEXT_R9(%rdi)
    decq    %rdx
    jz      2f

1:
    movq    12(%rsp, %rcx, 8), %rax
    movq    %rax, (%rsi, %rdx, 8)
    decq    %rdx
    jnz     1b
2:

    movq    $L(context_ret), (%rdi)
    ret

L(context_ret):
    /* load address of the context data structure */
    movq    (%rsp, %rbx, 8), %rdi

    /* save return value to the prev context */
    movq    %rax, KCONTEXT_RET(%rdi)
    movq    %rdi, %rax

    /* load address of next context data structure */
    movq    KCONTEXT_LINK(%rdi), %rdi
    cmpq    $0, %rdi
    je      L(bug)

    call    setcontext

L(bug):
    .word   BUG_VALUE

L(halt):
    rep;    nop
    jmp     L(halt)
SYM_CODE_END(makecontext)
