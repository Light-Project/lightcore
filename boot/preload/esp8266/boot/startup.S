/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <linkage.h>
#include <asm/entry.h>

    .section    .startup, "ax"
GLOBAL(_start)
    j       init
END(_start)

    .section    .text, "ax"
ENTRY(init)
    /* user vector mode */
    rsr     a0, ps
    movi    a1, 1<<5
    or      a0, a0, a1
    xor     a0, a0, a1
    wsr     a0, ps

    /* relocate vector */
    movi    a0, _start
    wsr     a0, vecbase

    /* enable timer int */
    rsr     a0, intenable
    movi    a1, 1<<9
    or      a0, a0, a1
    wsr     a0, intenable

    /* enable timer intc */
    movi    a0, 0x3ff00000
    movi    a1, 1<<1
    l32i    a2, a0, 0x04
    or      a2, a2, a1
    s32i    a2, a0, 0x04

    /* clear the bss region */
    movi    a2, 0x00
    movi    a3, _ld_bss_start
    movi    a4, _ld_bss_end
    j       Bss_loop
1:
    s32i    a2, a3, 0
    addi    a3, a3, 4
Bss_loop:
    bne     a3, a4, 1b

    /* set stack pointer */
    movi    sp, _ld_stack_end

    /* unmask interrupt */
    rsil    a0, 0

    call0   main
    j       halt
END(init)

ENTRY(kernel_handle)
    SAVE_REGS

    mov     a2, sp
    call0   timer_handle

    /* clear all interrupt */
    movi    a0, 0x3fff
    wsr     a0, intclear

    REC_REGS
    rfe
END(kernel_handle)

GLOBAL(kboot_start)
    callx0  a2
END(kboot_start)

GLOBAL(halt)
    rsil    a0, 15
    waiti   15
    j       halt
END(halt)

/*
 * Debug Exception.
 */
    .section    .Debug.Vector, "ax"
ENTRY(debug_vector)
    j       halt
END(debug_vector)

/*
 * NMI Exception.
 * Wdev interrupt.
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
    j       kernel_handle
END(kernel_vector)

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
