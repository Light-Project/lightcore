/* SPDX-License-Identifier: GPL-2.0 */
#include <linkage.h>

SYM_FUNC_START(memset)
    movq    %rdi,%r10

    /* expand byte value  */
    movzbl  %sil,%ecx
    movabs  $0x0101010101010101,%rax
    imulq   %rcx,%rax

    /* align dst */
    movl    %edi,%r9d
    andl    $7,%r9d
    jnz     L(bad_alignment)

L(after_bad_alignment):
    movq    %rdx,%rcx
    shrq    $6,%rcx
    jz      L(handle_tail)

    .p2align 4
L(loop_64):
    decq    %rcx
    movq    %rax,(%rdi)
    movq    %rax,8(%rdi)
    movq    %rax,16(%rdi)
    movq    %rax,24(%rdi)
    movq    %rax,32(%rdi)
    movq    %rax,40(%rdi)
    movq    %rax,48(%rdi)
    movq    %rax,56(%rdi)
    leaq    64(%rdi),%rdi
    jnz     L(loop_64)

    /*
     * Handle tail in loops. The loops should be faster than hard
     * to predict jump tables.
     */
    .p2align 4
L(handle_tail):
    movl    %edx,%ecx
    andl    $63&(~7),%ecx
    jz      L(handle_7)
    shrl    $3,%ecx

    .p2align 4
L(loop_8):
    decl    %ecx
    movq    %rax,(%rdi)
    leaq    8(%rdi),%rdi
    jnz     L(loop_8)

L(handle_7):
    andl    $7,%edx
    jz      L(ende)

    .p2align 4
L(loop_1):
    decl    %edx
    movb    %al,(%rdi)
    leaq    1(%rdi),%rdi
    jnz     L(loop_1)

L(ende):
    movq    %r10,%rax
    ret

L(bad_alignment):
    cmpq    $7,%rdx
    jbe     L(handle_7)
    movq    %rax,(%rdi)
    movq    $8,%r8
    subq    %r9,%r8
    addq    %r8,%rdi
    subq    %r8,%rdx
    jmp     L(after_bad_alignment)

SYM_FUNC_END(memset)

SYM_FUNC_START(memcpy)
    movq    %rdi, %rax

    cmpq    $0x20, %rdx
    jb      L(copy_handle_tail)

    /*
     * We check whether memory false dependence could occur,
     * then jump to corresponding copy mode.
     */
    cmp     %dil, %sil
    jl      L(copy_backward)
    subq    $0x20, %rdx

L(copy_forward_loop):
    subq    $0x20, %rdx

    /*
     * Move in blocks of 4x8 bytes:
     */
    movq    0*8(%rsi), %r8
    movq    1*8(%rsi), %r9
    movq    2*8(%rsi), %r10
    movq    3*8(%rsi), %r11
    leaq    4*8(%rsi), %rsi

    movq    %r8, 0*8(%rdi)
    movq    %r9, 1*8(%rdi)
    movq    %r10, 2*8(%rdi)
    movq    %r11, 3*8(%rdi)
    leaq    4*8(%rdi), %rdi
    jae     L(copy_forward_loop)
    addl    $0x20, %edx
    jmp     L(copy_handle_tail)

L(copy_backward):
    /*
     * Calculate copy position to tail.
     */
    addq    %rdx, %rsi
    addq    %rdx, %rdi
    subq    $0x20, %rdx

    /*
     * At most 3 ALU operations in one cycle,
     * so append NOPS in the same 16 bytes trunk.
     */
    .p2align 4
L(copy_backward_loop):
    subq    $0x20, %rdx
    movq    -1*8(%rsi), %r8
    movq    -2*8(%rsi), %r9
    movq    -3*8(%rsi), %r10
    movq    -4*8(%rsi), %r11
    leaq    -4*8(%rsi), %rsi
    movq    %r8, -1*8(%rdi)
    movq    %r9, -2*8(%rdi)
    movq    %r10, -3*8(%rdi)
    movq    %r11, -4*8(%rdi)
    leaq    -4*8(%rdi), %rdi
    jae     L(copy_backward_loop)

    /*
     * Calculate copy position to head.
     */
    addl    $0x20, %edx
    subq    %rdx, %rsi
    subq    %rdx, %rdi

L(copy_handle_tail):
    cmpl    $16, %edx
    jb      L(less_16bytes)

    /*
     * Move data from 16 bytes to 31 bytes.
     */
    movq    0*8(%rsi), %r8
    movq    1*8(%rsi), %r9
    movq    -2*8(%rsi, %rdx), %r10
    movq    -1*8(%rsi, %rdx), %r11
    movq    %r8, 0*8(%rdi)
    movq    %r9, 1*8(%rdi)
    movq    %r10, -2*8(%rdi, %rdx)
    movq    %r11, -1*8(%rdi, %rdx)
    ret

    .p2align 4
L(less_16bytes):
    cmpl    $8, %edx
    jb      L(less_8bytes)

    /*
     * Move data from 8 bytes to 15 bytes.
     */
    movq    0*8(%rsi), %r8
    movq    -1*8(%rsi, %rdx), %r9
    movq    %r8, 0*8(%rdi)
    movq    %r9, -1*8(%rdi, %rdx)
    ret

    .p2align 4
L(less_8bytes):
    cmpl    $4, %edx
    jb      L(less_3bytes)

    /*
     * Move data from 4 bytes to 7 bytes.
     */
    movl    (%rsi), %ecx
    movl    -4(%rsi, %rdx), %r8d
    movl    %ecx, (%rdi)
    movl    %r8d, -4(%rdi, %rdx)
    ret

    .p2align 4
L(less_3bytes):
    subl    $1, %edx
    jb      L(end)

    /*
     * Move data from 1 bytes to 3 bytes.
     */
    movzbl  (%rsi), %ecx
    jz      L(store_1byte)
    movzbq  1(%rsi), %r8
    movzbq  (%rsi, %rdx), %r9
    movb    %r8b, 1(%rdi)
    movb    %r9b, (%rdi, %rdx)

L(store_1byte):
    movb    %cl, (%rdi)

L(end):
    ret
SYM_FUNC_END(memcpy)
