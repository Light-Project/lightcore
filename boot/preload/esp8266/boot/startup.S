/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <linkage.h>

    .section    .startup, "ax"
GLOBAL(_start)
    j       init
END(_start)

    .section    .text, "ax"
ENTRY(init)
Bss_segment_Init:
    movi    a2, 0x00
    movi    a3, _ld_sbss
    movi    a4, _ld_ebss
    j       Bss_loop
1:
    s32i    a2, a3, 0
    addi    a3, a3, 4
Bss_loop:
    bne     a3, a4, 1b

    /* User vector mode */
    rsr     a0, ps
    movi    a1, 1<<5
    or      a0, a0, a1
    xor     a0, a0, a1
    wsr     a0, ps

    /* relocate vector */
    movi    a0, _start
    wsr     a0, vecbase

    /* enable timer intc */
    movi    a0, 0x3ff00000
    movi    a1, 1<<1
    l32i    a2, a0, 0x04
    or      a2, a2, a1
    s32i    a2, a0, 0x04

    /* enable timer int */
    rsr     a0, intenable
    movi    a1, 1<<9
    or      a0, a0, a1
    wsr     a0, intenable

    /* unmask interrupt */
    rsil    a0, 0

env:
    movi    sp, _ld_stack_end
    call0   main
    j       halt
END(init)

GLOBAL(kernel_start)
    movi    sp, CONFIG_XIP_BASE
    callx0  sp
END(kernel_start)

/* Mask interrupt and stop */
ENTRY(halt)
    rsil    a0, 15
    waiti   15
    j       halt
END(halt)

    .section    .data
ENTRY(int_table)
    .long   0   /* interruptee's sp */
    .long   0   /* interruptee's ps */
    .long   0   /* interruptee's pc */
    .long   0   /* interruptee's a0 */
    .long   0   /* interruptee's sar */
    .long   0   /* interruptee's a2 */
    .long   0   /* interruptee's a3 */
    .long   0   /* interruptee's a4 */
    .long   0   /* interruptee's a5 */
    .long   0   /* interruptee's a6 */
    .long   0   /* interruptee's a7 */
    .long   0   /* interruptee's a8 */
    .long   0   /* interruptee's a9 */
    .long   0   /* interruptee's a10 */
    .long   0   /* interruptee's a11 */
    .long   0   /* interruptee's a12 */
    .long   0   /* interruptee's a13 */
    .long   0   /* interruptee's a14 */
    .long   0   /* interruptee's a15 */
    .long   0   /* interruptee's  */
    .long   0   /* interruptee's  */
END(int_table)

/*
 * Debug Exception.
 *
 */
    .section    .Debug.Vector, "ax"
ENTRY(debug_vector)
    j       halt
END(debug_vector)

/*
 * NMI Exception.
 * wdev interrupt.
 */
    .section    .NMI.Vector, "ax"
ENTRY(nmi_vector)
    j       halt
END(nmi_vector)

/*
 * Kernel Exception.
 * All external interrupts and syscall.
 */
    .section    .Kernel.Vector, "ax"
ENTRY(kernel_vector)
    wsr         a0, excsave1
    j           kernel_handle
END(kernel_vector)

    .section    .text, "ax"
ENTRY(kernel_handle)
    /* Save context */
    movi        a0, int_table
    s32i        sp, a0, 0x00

    rsr         sp, ps
    s32i        sp, a0, 0x04

    rsr         sp, epc1
    s32i        sp, a0, 0x08

    rsr         sp, excsave1
    s32i        sp, a0, 0x0c

    rsr         sp, sar
    s32i        sp, a0, 0x10

    s32i        a2, a0, 0x14
    s32i        a3, a0, 0x18
    s32i        a4, a0, 0x1c
    s32i        a5, a0, 0x20
    s32i        a6, a0, 0x24
    s32i        a7, a0, 0x28
    s32i        a8, a0, 0x2c
    s32i        a9, a0, 0x30
    s32i        a10, a0, 0x34
    s32i        a11, a0, 0x38
    s32i        a12, a0, 0x3c
    s32i        a13, a0, 0x40
    s32i        a14, a0, 0x44
    s32i        a15, a0, 0x48

    l32i        sp, a0, 0x00
    call0       timer_handle

    /* clear all interrupt */
    movi        a0, 0x3fff
    wsr         a0, intclear

    /* Recovery context */
    movi        a0, int_table
    l32i        a15, a0, 0x48
    l32i        a14, a0, 0x44
    l32i        a13, a0, 0x40
    l32i        a12, a0, 0x3c
    l32i        a11, a0, 0x38
    l32i        a10, a0, 0x34
    l32i        a9, a0, 0x30
    l32i        a8, a0, 0x2c
    l32i        a7, a0, 0x28
    l32i        a6, a0, 0x24
    l32i        a5, a0, 0x20
    l32i        a4, a0, 0x1c
    l32i        a3, a0, 0x18
    l32i        a2, a0, 0x14

    l32i        sp, a0, 0x10
    wsr         sp, sar

    l32i        sp, a0, 0x08
    wsr         sp, epc1

    l32i        sp, a0, 0x04
    wsr         sp, ps

    l32i        sp, a0, 0x00
    l32i        a0, a0, 0x0c

    rsync
    rfe
END(kernel_handle)

/*
 * User Exception.
 * We don't use user exceptions.
 */
    .section    .User.Vector, "ax"
ENTRY(user_vector)
    j       halt
END(user_vector)

/*
 * Double Exception.
 * Double exceptions are not a normal occurrence.
 * They indicate a bug of some kind.
 */
    .section    .Double.Vector, "ax"
ENTRY(double_vector)
    j       halt
END(double_vector)

/*
 * Reset Exception.
 * The vecbase is restored when hardware reset,
 * so this vector is useless. It's just to show chip design.
 */
    .section    .Reset.Vector, "ax"
ENTRY(reset_vector)
    j       init
END(reset_vector)
