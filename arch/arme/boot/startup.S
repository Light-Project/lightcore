/*
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  arch/arme/boot/boot.S
 *  Arme serial boot
 *  (C) 2020 wzl
 * 
 *  Change Logs:
 *  Date            Notes
 *  2021-02-26      first version 
 * 
 */
 
    .syntax unified
    .thumb
    .section    .startup,"ax"
    .type       startup_head, %object
    .size       startup_head, .-startup_head
    
.global startup_head
startup_head:
    .long  _ld_estack       /* 0x00: arm sp   */
    .long  init             /* 0x04: arm jump */
    .long  0, 0             /* 0x08: Reserved */
    .long  0, 0, 0, 0       /* 0x10: boot parameter */
    .asciz "lightcore!"     /* 0x20: system magic number */
    .byte  0                /* 0x0a: Reserved */
    .long  0                /* 0x0c: Image crc */
    .long  _ld_kernel_size  /* 0x30: Kernel size */
    .long  0, 0, 0          /* 0x34: Reserved */
    
    .section    .init.text,"a"
    .type       init, %function
.global init
init:


Data_segment_Init:
    ldr     r0, =_ld_rodata_start
    ldr     r1, =_ld_sdata
    ldr     r2, =_ld_edata
    b       Data_loop
1:
    ldr     r3, [r0]
    str     r3, [r1]
    adds    r0, #4
    adds    r1, #4
Data_loop:
    cmp     r1, r2
    bcc     1b

Bss_segment_Init:
    movs    r0, #0
    ldr     r2, = _ld_bss_start
    ldr     r3, = _ld_bss_end
    b       Bss_loop
1:
    str     r0, [r2]
    adds    r2, #4
Bss_loop:
    cmp     r2, r3
    bcc     1b
        
    bl  kernel_start
    
halt:
    wfi
    b   halt

    
