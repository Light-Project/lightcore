/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <linkage.h>
#include <arch/x86/seg.h>
#include <arch/x86/msr.h>
#include <arch/x86/regs.h>

    .code32
    .section .text, "ax"

SYM_FUNC_START(startup32)
    /* Setup segments values */
    movl    $GDT_ENTRY_BOOT_DS_BASE, %eax
    movl    %eax, %ds
    movl    %eax, %es
    movl    %eax, %ss

    /* clear bss first */
    xorl    %eax, %eax
    movl    $_ld_bss_start, %edi
    movl    $_ld_bss_end, %ecx
    subl    %edi, %ecx
    shrl    $2, %ecx
    rep;    stosl %eax, %es:(%edi)

#ifdef CONFIG_ARCH_X86_64

    /* Reload long mode GDT */
    lgdtl   gdt_struct

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
#else

    .align  16
SYM_DATA_START_LOCAL(gdt_table)
    .long   0x00000000      /* 0x00: NULL ENTRY                     */
    .long   0x00000000      /* 0x04: NULL                           */
    .long   0x0000ffff      /* 0x08: CS32 ENTRY                     */
    .long   0x00cf9a00      /* 0x0c: code, read/exe, 4GB, base=0    */
    .long   0x0000ffff      /* 0x10: CS ENTRY                       */
    .long   0x00af9a00      /* 0x14: code, read/exe, 4GB, base=0    */
    .long   0x0000ffff      /* 0x18: DS ENTRY                       */
    .long   0x00cf9200      /* 0x1c: data, read/write, 4GB, base=0  */
gdt_struct:
    .word   0x001f          /* GDT Limit    */
    .long   gdt_table       /* GDT Addr     */
SYM_DATA_END(gdt_table)
#endif
