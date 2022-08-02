/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <linkage.h>
#include <kboot.h>
#include <arch/x86/seg.h>
#include <arch/x86/regs.h>

#define io_delay \
    outb    %al, $0x80

    .code16
    .section .text, "ax"

SYM_FUNC_START(startup16)
    /* Setup segments values */
    xorl    %eax, %eax
    movl    %eax, %ds
    movl    %eax, %es

    /* Disable NMI */
    movb    $0x80, %al
    outb    %al, $0x70
    io_delay

    /* Reset IGNNE# if asserted in the FPU */
    movb    $0xf0, %al
    outb    %al, $0xa1
    io_delay
    movb    $0xf1, %al
    outb    %al, $0xa1
    io_delay

    /* Disable all interrupts at the legacy PIC */
    movb    $0xff, %al
    outb    %al, $0xa1
    io_delay
    movb    $0xfb, %al
    outb    %al, $0x21
    io_delay

    /* Setup GDT table */
    lgdtl   gdt32_table

    /* Setup IDT table */
    lidtl   idt_table

    /* Enable Protected mode */
    movl    %cr0, %eax
    orl     $CR0_PE, %eax
    movl    %eax, %cr0

    /* Transition to 32-bit mode */
    ljmpl   $GDT_ENTRY_BOOT_CS_BASE, $startup32

SYM_FUNC_END(startup16)

    .align  16
SYM_DATA_START_LOCAL(idt_table)
    .word   0x0000          /* IDT Limit    */
    .long   0x00000000      /* IDT Addr     */
SYM_DATA_END(idt_table)
