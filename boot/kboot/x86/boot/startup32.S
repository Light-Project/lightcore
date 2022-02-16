/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <linkage.h>
#include <arch/x86/seg.h>

    .code32
    .align  32
ENTRY(startup32)
/*
 * Set segments to known values
 */
    movl    $GDT_ENTRY_BOOT_DS_BASE, %eax
    movl    %eax, %ds
    movl    %eax, %es
    movl    %eax, %ss

/*
 * Initialize got table
 */
    leal    _ld_got_start(%ebx), %edi
    leal    _ld_got_end(%ebx), %ecx
    subl    %edi, %ecx
    shrl    $2, %ecx
1:
    movl    (%edi), %eax
    addl    %ebx, %eax
    stosl   %eax, %es:(%edi)
    loop    1b

/*
 * Clear the BSS region
 */
    leal    _ld_bss_start(%ebx), %edi
    leal    _ld_bss_end(%ebx), %ecx
    subl    %edi, %ecx
    shrl    $2, %ecx
    xorl    %eax, %eax
    rep;    stosl %eax, %es:(%edi)

    movl    $_ld_stack_end, %esp
    addl    %ebx, %esp

#ifdef CONFIG_ARCH_X86_32
    xorl    %ebp, %ebp
    call    main
    jmp     halt
#else
    jmp     startup_64
#endif
END(startup32)

GLOBAL(halt)
    rep;    hlt
    jmp     halt
END(halt)

GLOBAL(kernel_start)
    movl    4(%esp), %ecx
    movl    8(%esp), %edx

    call    1f
1:  movl    (%esp), %eax

    movw    %cx, (3f - 1b)(%eax)
    movl    %edx, (2f - 1b)(%eax)

    .byte   0xea    /* ljmpl opcode */
2:  .long   0x00    /* address */
3:  .word   0x00    /* segment */
END(kernel_start)
