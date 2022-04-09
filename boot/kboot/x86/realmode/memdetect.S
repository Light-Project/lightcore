/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <linkage.h>
#include <asm/e820.h>
#include <arch/x86/regs.h>
#include "e820-offset.h"

#define E820_MAGIC 0xe820
#define E820_SMAP 0x534d4150

    .code16
    .section .text, "ax"

SYM_FUNC_START(memdetect)
    xorl    %edx, %edx
    xorl    %ebx, %ebx
restart:

    pushl   %edx
    movl    $E820_MAGIC, %eax
    movl    $E820_SMAP, %edx
    movl    $e820_buffer, %edi
    movl    $e820_end - e820_buffer, %ecx
    int     $0x15
    popl    %edx

    jb      e820_finish
    cmpl    $E820_SMAP, %eax
    jne     e820_error

    pushl   %edx
    movl    $e820_end - e820_buffer, %ecx
    movl    $e820_buffer, %esi
    movl    $bootparam, %edi
    movl    %ecx, %eax
    mull    %edx
    leal    E820_ENTRY(%edi), %edi
    addl    %eax, %edi
    shrl    $2, %ecx
    rep;    movsl %es:(%esi), %es:(%edi)
    popl    %edx

    incl    %edx
    testl   %ebx, %ebx
    jz      e820_finish
    cmp     $E820_MAX_ENTRIES, %edx
    jl      restart

e820_finish:
    movl    $bootparam, %ebx
    movl    %edx, E820_NUM(%ebx)

e820_error:
    ret
SYM_FUNC_END(memdetect)

SYM_DATA_START_LOCAL(e820_buffer)
    .quad 0x0000000000000000
    .quad 0x0000000000000000
    .long 0x00000000
SYM_DATA_END_LABEL(e820_buffer, SYM_L_LOCAL, e820_end)
