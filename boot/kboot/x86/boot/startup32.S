/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 John Sanpe <sanpeqf@gmail.com>
 */

#include <linkage.h>
#include <arch/x86/seg.h>
#include <arch/x86/msr.h>
#include <arch/x86/regs.h>

    .code32
    .section .text, "ax"

SYM_FUNC_START(startup32)
    /* Setup segments for flat 32-bit mode */
    movl    $GDT_ENTRY_BOOT_DS_BASE, %eax
    movl    %eax, %ds
    movl    %eax, %es
    movl    %eax, %ss

    /* Clear the bss segment */
    xorl    %eax, %eax
    movl    $_ld_bss_start, %edi
    movl    $_ld_bss_end, %ecx
    subl    %edi, %ecx
    shrl    $2, %ecx
    rep;    stosl %eax, %es:(%edi)

#ifdef CONFIG_ARCH_X86_64

    /* Reload long mode GDT */
    lgdtl   gdt64_table

    pushl   $GDT_LENTRY_KERNEL32_CS_BASE
    pushl   $1f
    lretl
1:

    movl    $GDT_LENTRY_KERNEL_DS_BASE, %eax
    movl    %eax, %ds
    movl    %eax, %es
    movl    %eax, %ss

    /* Enable PAE mode */
    movl    %cr4, %eax
    orl     $CR4_PAE, %eax
    movl    %eax, %cr4

    /* Build PMD (first entry) */
    movl    $0x183, page_pmd

    /* Build PUD (first entry) */
    movl    $page_pmd, %edx
    orl     $0x7, %edx
    movl    %edx, page_pud

    /* Build PGD (first entry) */
    movl    $page_pud, %edx
    orl     $0x7, %edx
    movl    %edx, page_dir

    /* Enable boot page table */
    movl    $page_dir, %eax
    movl    %eax, %cr3

    /* Enable Long mode in EFER */
    movl    $MSR_EFER, %ecx
    rdmsr
    btsl    $_EFER_LME, %eax
    wrmsr

    /* Paging must be enabled in long mode */
    movl    %cr0, %eax
    orl     $CR0_PG, %eax
    movl    %eax, %cr0

    /* Jump to long mode */
    pushl   $GDT_LENTRY_KERNEL_CS_BASE
    pushl   $startup64
    lretl

#else
    /* Initialization stack */
    movl    $_ld_stack_end, %esp
    xorl    %ebp, %ebp

    call    main
    jmp     halt
#endif

SYM_FUNC_END(startup32)

#ifdef CONFIG_ARCH_X86_32
SYM_FUNC_START(halt)
    rep;    hlt
    jmp     halt
SYM_FUNC_END(halt)

SYM_FUNC_START(kernel_start)
    movl    4(%esp), %ecx
    movl    8(%esp), %edx
    pushl   %ecx
    pushl   %edx
    lretl
SYM_FUNC_END(kernel_start)
#endif
