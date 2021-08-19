/*
 *  SPDX-License-Identifier: GPLV2
 *  lightcore/arch/arm/926ejs/boot/boot.S
 *  Arm serial boot
 *  (C) 2020 wzl
 */

#include <asm-generic/header.h>
#include <asm/page.h>
#include <linkage.h>

    .section	.startup,"ax"
GLOBAL(boot_head)
    ldr    pc, =init        /* 0x00: arm jump */
    .long  0, 0, 0          /* 0x04: Reserved */
    KERNEL_HEADER
END(boot_head)

.word       _ld_bss_start
.word       _ld_bss_end

    .section    .init.text.Reset_Handler,"a"
    .type       Reset_Handler, %function
ENTRY(init)
    mrs     r0, cpsr
    bic     r0, #0x1f
    orr     r0, #0x13
    msr     cpsr, r0
    
Bss_segment_Init:
    movs    r0, #0
    ldr     r1, =_ld_bss_start
    ldr     r2, =_ld_bss_end
    b       Bss_loop
1:
    str     r0, [r1], #4
Bss_loop:
    cmp     r1, r2
    bcc     1b

    ldr     sp, =(init_thread_union + THREAD_SIZE)
    
    bl      kernel_start    
END(init)

ENTRY(halt)
    b       halt
END(halt)
