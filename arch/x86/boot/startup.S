/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <asm-generic/header.h>
#include <linkage.h>
#include <asm/page.h>
#include <arch/x86/seg.h>

    .section    .startup, "ax"
GLOBAL(boot_head)
    ljmpl   $GDT_ENTRY_KERNEL_CS_BASE, $init    /* 0x00: x86 jump */
    .align  16                                  /* ----: Reserved */
    KERNEL_HEADER
END(boot_head)

    .section    .text.init,"ax"
ENTRY(init)
    movw    $GDT_ENTRY_KERNEL_DS_BASE, %di
    xorl    %eax, %eax

    /* Set segment selector */
    movw    %di, %ss    /* Stack segment */
    movw    %di, %ds    /* Data segment */
    movw    %di, %es    /* Extra segment */
    movw    %ax, %fs    /* File segment */
    movw    %ax, %gs    /* Global segment */

    /* Clear bss */
    movl    $_ld_bss_start, %edi
    movl    $_ld_bss_end, %ecx 
    subl    %edi, %ecx
    shrl    $2, %ecx
    rep;    stosl %eax, %es:(%edi)

    movl    $init_thread_union, %esp
    addl    $THREAD_SIZE, %esp

    call    kernel_start
END(init)

ENTRY(halt)
    cli
    rep;    hlt
    jmp     halt
END(halt)
