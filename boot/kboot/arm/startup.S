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
    ldr     sp, =_ld_stack_end
    mov     lr, #0
    b       main
END(_start)

GLOBAL(kernel_start)
    mov     pc, a1
END(kernel_start)
