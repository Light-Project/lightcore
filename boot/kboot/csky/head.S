/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 John Sanpe <sanpeqf@gmail.com>
 */

#include <linkage.h>

    .section .startup,"ax"
GLOBAL(_start)
    lrw     r1, _ld_stack_end
    mov     sp, r1

    bsr     main
    br      halt
END(_start)

GLOBAL(kernel_start)
    jmp     a0
END(kernel_start)

GLOBAL(halt)
    stop
    br      halt
END(halt)
