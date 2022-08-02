/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <linkage.h>
#include <arch/x86/seg.h>

    .code64
    .section .text, "ax"

SYM_FUNC_START(startup64)
    /* Setup segments values */
    xorl    %eax, %eax
    movl    %eax, %ds
    movl    %eax, %es
    movl    %eax, %ss

    /* Initialization stack */
    movq    $_ld_stack_end, %rsp
    xorq    %rbp, %rbp

    call    main
    jmp     halt
SYM_FUNC_END(startup64)

SYM_FUNC_START(halt)
    rep;    hlt
    jmp     halt
SYM_FUNC_END(halt)

SYM_FUNC_START(kernel_start)
    pushq   %rdi
    pushq   %rsi
    lretq
SYM_FUNC_END(kernel_start)
