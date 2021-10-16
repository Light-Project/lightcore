/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <linkage.h>
#include <arch/x86/seg.h>

    .code16
    .section    .text, "ax"
SYM_CODE_START(_start)
    wbinvd
    cli

    movw    %cs, %ax
    movw    %ax, %dx
    movw    $1, %ax
    lmsw    %ax

    ljmpl   $GDT_ENTRY_KERNEL_CS_BASE, $init
SYM_FUNC_END(_start)

SYM_CODE_START(init)


SYM_FUNC_END(init)
