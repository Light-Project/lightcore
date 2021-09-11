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
    // EFI_HEADER
END(head)

GLOBAL(_start)
    ldr     sp, =_ld_stack_end

    bl      main
    b       halt
END(_start)

GLOBAL(kernel_start)
    mov     pc, a1
END(kernel_start)

ENTRY(halt)
    nop
    b   halt
END(halt)
