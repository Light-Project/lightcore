/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <linkage.h>
#include <arch/x86/seg.h>

    .code16
    .section    .text, "ax"

GLOBAL(smp_start)
    wbinvd

    cli

    movw    %cs, %ax
    movw    %ax, %dx

    movw    $1, %ax
    lmsw    %ax

    ljmpl   $GDT_ENTRY_KERNEL_CS_BASE, $init
END(smp_start)
