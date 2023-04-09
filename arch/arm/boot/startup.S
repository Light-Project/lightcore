/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 John Sanpe <sanpeqf@gmail.com>
 */

#include <linkage.h>
#include <asm-generic/header.h>
#include <asm/page.h>

    .section	.startup, "ax"
GLOBAL(boot_head)
    ldr    pc, =init        /* 0x00: arm jump */
    .long  0, 0, 0          /* 0x04: Reserved */
    KERNEL_HEADER
END(boot_head)

    .section    .init.text, "ax"
ENTRY(init)

    ldr     sp, =(init_thread_union + THREAD_SIZE)

/* clear bss segment */
    ldr     a1, =_ld_bss_start
    ldr     a3, =_ld_bss_end
    sub     a3, a1
    mov     a2, #0
    bl      memset

    mov     lr, #0
    b       kernel_start
END(init)
