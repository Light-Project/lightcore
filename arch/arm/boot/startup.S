/*
 *  SPDX-License-Identifier: GPLV2
 *  lightcore/arch/arm/926ejs/boot/boot.S
 *  Arm serial boot
 *  (C) 2020 wzl
 */

#include <asm/page.h>
#include <linkage.h>
 
    .arm
    .section	.startup,"ax"
    
GLOBAL(startup_head)
    ldr    pc, =init        /* 0x00: arm jump */
    .long  0, 0, 0          /* 0x04: Reserved */
    .long  0, 0, 0, 0       /* 0x10: boot parameter */
    .asciz "lightcore!"     /* 0x20: system magic number */
    .byte  0                /* 0x0a: Reserved */
    .long  0                /* 0x0c: Image crc */
    .long  _ld_kernel_size  /* 0x30: Kernel size */
    .long  0, 0, 0          /* 0x34: Reserved */
    
END(startup_head)

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

env:
    ldr     sp, =(init_thread_union + THREAD_SIZE)
    
    ldr     ip, =startup_head
    ldr     r0, [ip, #0x10]
    
    bl      kernel_start
    
END(init)
