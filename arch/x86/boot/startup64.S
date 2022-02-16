/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <linkage.h>
#include <asm/page.h>
#include <arch/x86/seg.h>
#include <asm-generic/header.h>

    .code64
    .section    .startup, "ax"
GLOBAL(boot_head)
    jmp     init        /* 0x00: x86 jump */
    .align  16          /* ----: Reserved */
    KERNEL_HEADER
END(boot_head)

    .section    .text.init,"ax"
ENTRY(init)
    xorq    %rax, %rax
    movw    $(GDT_ENTRY_KERNEL_DS_BASE), %dx

    /* set segment selector */
    movw    %dx, %ss    /* Stack segment    */
    movw    %dx, %ds    /* Data segment     */
    movw    %dx, %es    /* Extra segment    */
    movw    %ax, %fs    /* File segment     */
    movw    %ax, %gs    /* Global segment   */

    /* clear bss first */
    movq    $(_ld_bss_start), %rdi
    movq    $(_ld_bss_end), %rcx
    subq    %rdi, %rcx
    shrq    $4, %rcx
    rep;    stosq %rax, %es:(%rdi)

    movq    $init_thread_union, %rbx
    leaq    THREAD_SIZE(%ebx), %rsp

    jmp     L(startup_kernel)
END(init)

SYM_FUNC_START(startup_smp)
    xorl    %eax, %eax
    movw    $(GDT_ENTRY_KERNEL_DS_BASE), %dx

    /* set segment selector */
    movw    %dx, %ss    /* Stack segment    */
    movw    %dx, %ds    /* Data segment     */
    movw    %dx, %es    /* Extra segment    */
    movw    %ax, %fs    /* File segment     */
    movw    %ax, %gs    /* Global segment   */

L(startup_kernel):
    pushq   $0
    popfq

    /* clear ldt */
    xorw    %ax, %ax
    lldt    %ax

    xorq    %rbp, %rbp
    call    kernel_start

L(halt):
    rep;    hlt
    jmp     L(halt)
SYM_FUNC_END(startup_smp)
