/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <asm-generic/header.h>
#include <asm/page.h>
#include <asm/regs.h>
#include <linkage.h>

    .section    .startup,"ax"
GLOBAL(boot_head)
    br      init        /* 0x00: csky jmp */
    .byte   0, 0        /* 0x02: Reserved */
    .long   0, 0, 0     /* 0x04: Reserved */
    KERNEL_HEADER
END(boot_head)

ENTRY(init)
    xor     r1, r1
    lrw     l0, _ld_bss_start
    lrw     l1, _ld_bss_end
    br      Bss_loop
1:
    st.w    r1, (l0, 0)
    addi    l0, 4
Bss_loop:
    cmphs   l0, l1
    bf      1b

    lrw     r1, init_thread_union + THREAD_SIZE
    mov     sp, r1

    movi    l0, 0
    jsri    kernel_start
    br      halt
END(init)

ENTRY(halt)
    stop
    br      halt
END(halt)
