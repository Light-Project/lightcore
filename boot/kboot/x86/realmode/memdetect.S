/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <linkage.h>
#include <asm/e820.h>
#include <arch/x86/regs.h>
#include "e820-offset.h"

#define E820_MAGIC  0xe820
#define E820_SMAP   0x534d4150
#define E820_BSIZE  0x14
#define E820_INTN   0x15

    .code16
    .section .text, "ax"

SYM_FUNC_START(memdetect)
    xorl    %edx, %edx
    xorl    %ebx, %ebx
    subl    $E820_BSIZE, %esp

L(e820_restart):
    movl    $E820_MAGIC, %eax
    movl    $E820_BSIZE, %ecx
    movl    %esp, %edi

    pushl   %edx
    movl    $E820_SMAP, %edx
    int     $E820_INTN
    popl    %edx

    jb      L(e820_finish)
    cmpl    $E820_SMAP, %eax
    jne     L(e820_error)

    movl    $E820_BSIZE, %ecx
    movl    $bootparam, %edi
    movl    %esp, %esi
    movl    %ecx, %eax

    pushl   %edx
    mull    %edx
    leal    E820_ENTRY(%edi), %edi
    addl    %eax, %edi
    shrl    $2, %ecx
    rep;    movsl %es:(%esi), %es:(%edi)
    popl    %edx

    incl    %edx
    testl   %ebx, %ebx
    jz      L(e820_finish)
    cmp     $E820_MAX_ENTRIES, %edx
    jl      L(e820_restart)

L(e820_finish):
    movl    $bootparam, %ebx
    movl    %edx, E820_NUM(%ebx)

L(e820_error):
    addl    $E820_BSIZE, %esp
    ret
SYM_FUNC_END(memdetect)
