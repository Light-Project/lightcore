/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 John Sanpe <sanpeqf@gmail.com>
 */

#include <linkage.h>
#include <asm/page.h>
#include <arch/x86/seg.h>
#include <asm-generic/header.h>

    .code64
    .section .startup, "ax"

GLOBAL(boot_head)
    jmp     init        /* 0x00: x86 jump */
    .align  16          /* ----: Reserved */
    KERNEL_HEADER
END(boot_head)

    .section .init.text,"ax"

SYM_CODE_START_LOCAL(init)
    xorq    %rax, %rax

    /* set segment selector */
    movw    %ax, %ds    /* Data segment     */
    movw    %ax, %es    /* Extra segment    */
    movw    %ax, %ss    /* Stack segment    */
    movw    %ax, %fs    /* File segment     */
    movw    %ax, %gs    /* Global segment   */

    pushq   $GDT_LENTRY_KERNEL_CS_BASE
    pushq   $L(kernel_cs)
    lretq

L(kernel_cs):

    /* clear bss first */
    movq    $(_ld_bss_start), %rdi
    movq    $(_ld_bss_end), %rcx
    subq    %rdi, %rcx
    shrq    $3, %rcx
    rep;    stosq %rax, %es:(%rdi)

    movq    $init_thread_union, %rbx
    leaq    THREAD_SIZE(%rbx), %rsp

    jmp     L(startup_kernel)
SYM_CODE_END(init)

SYM_FUNC_START(startup_smp)
    /* set segment selector */
    xorw    %ax, %ax
    movw    %ax, %ds    /* Data segment     */
    movw    %ax, %es    /* Extra segment    */
    movw    %ax, %ss    /* Stack segment    */
    movw    %ax, %fs    /* File segment     */
    movw    %ax, %gs    /* Global segment   */

L(startup_kernel):
    pushq   $0
    popfq

    xorq    %rbp, %rbp
    call    kernel_start

L(halt):
    rep;    hlt
    jmp     L(halt)
SYM_FUNC_END(startup_smp)
