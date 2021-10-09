/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <linkage.h>
#include <size.h>

#include "efi-header.S"

    .section .head,"ax"
GLOBAL(head)
.rept   7
    MZ_HEADER
.endr
    b   _start
END(head)

GLOBAL(_start)

    /*
	 * Booting from Angel - need to enter SVC mode and disable
	 * FIQs/IRQs (numeric definitions from angel arm.h source).
	 * We only do this if we were in user mode on entry.
	 */
    mrs     v1, cpsr
    tst	    v1, #3
    bne     not_angel
    mov     a1, #0x17
    swi     #0x123456
not_angel:

    /* Find the start of physical memory */
    mov     a1, pc
    mov     a1, #0xf8000000

    ldr     sp, =_ld_stack_end
    mov     lr, #0
    b       main
END(_start)

GLOBAL(kernel_start)
    mov     pc, a1
END(kernel_start)
