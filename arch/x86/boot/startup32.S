/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <linkage.h>
#include <asm/page.h>
#include <arch/x86/seg.h>
#include <asm-generic/header.h>

    .code32
    .section    .startup, "ax"
GLOBAL(boot_head)
    jmp     init        /* 0x00: x86 jump */
    .align  16          /* ----: Reserved */
    KERNEL_HEADER
END(boot_head)

    .section    .init.text,"ax"
ENTRY(init)
    xorl    %eax, %eax
    movw    $(GDT_ENTRY_KERNEL_DS_BASE), %dx

    /* set segment selector */
    movw    %dx, %ss    /* Stack segment    */
    movw    %dx, %ds    /* Data segment     */
    movw    %dx, %es    /* Extra segment    */
    movw    %ax, %fs    /* File segment     */
    movw    %ax, %gs    /* Global segment   */

    /* clear bss first */
    movl    $(_ld_bss_start), %edi
    movl    $(_ld_bss_end), %ecx
    subl    %edi, %ecx
    shrl    $2, %ecx
    rep;    stosl %eax, %es:(%edi)

    movl    $init_thread_union, %ebx
    leal    THREAD_SIZE(%ebx), %esp

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
    pushl   $0
    popfl

    /* clear ldt */
    xorw    %ax, %ax
    lldt    %ax

    xorl    %ebp, %ebp
    call    kernel_start

L(halt):
    rep;    hlt
    jmp     L(halt)
SYM_FUNC_END(startup_smp)
