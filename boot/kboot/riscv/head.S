/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */
#include <linkage.h>

    .section .startup,"ax"
GLOBAL(_start)
    lrw     r1, _ld_stack_end
    mov     r0, r1

    jmpi    main
    jmpi    halt
END(_start)

GLOBAL(kernel_start)
    jmp     a1
END(kernel_start)

GLOBAL(halt)
    stop
    jmpi    halt
END(halt)
