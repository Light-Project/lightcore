/*
 *  SPDX-License-Identifier: GPLV2
 *  lightcore/arch/arm/926ejs/boot/boot.S
 *  X86 serial boot
 *  (C) 2020 wzl
 */
 
#include <asm-generic/header.h>
#include <linkage.h>
#include <asm/page.h>
#include <arch/x86/seg.h>

    .section    .startup,"ax"
GLOBAL(startup_head)
    ljmpl   $GDT_ENTRY_KERNEL_CS_BASE, $init    /* 0x00: x86 jump */
    .align  16                                  /* ----: Reserved */
    KERNEL_HEADER
END(startup_head)

    .section    .text.init,"ax"
ENTRY(init)

    /* Set data selector */
    movw    $GDT_ENTRY_KERNEL_DS_BASE, %ax    
    movw    %ax, %ds
    movw    %ax, %es
    movw    %ax, %ss

    xorl    %eax, %eax
    movw    %ax, %fs
    movw    %ax, %gs

Bss_segment_Init:
    movl    $_ld_bss_start, %edi
    movl    $_ld_bss_end, %ecx 
    subl    %edi, %ecx
    rep;    stosl

env:
    movl    $init_thread_union, %esp
    addl    $THREAD_SIZE, %esp
    pushl   startup_head + 0x10
    call    kernel_start

halt:
    cli
    rep;    hlt
    jmp     halt

END(init)
