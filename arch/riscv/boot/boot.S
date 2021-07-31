/*
 *  SPDX-License-Identifier: GPLV2
 *  lightcore/arch/arm/926ejs/boot/boot.S
 *  riscv64 serial boot
 *  (C) 2020 wzl
 */
 
#include <generated/autoconf.h>

    .arm
    .section	.startup,"a"
    .type	boot_head, %object
    .size	boot_head, .-boot_head

.section .text
.global _entry

_entry:

    /* disable interrupts */
    csrw    mideleg, zero
    csrw    medeleg, zero
    csrw    mie, zero
    csrw    mip, zero
    
    /* reset registers */
    mov     ra,  zero
    mov     sp,  zero
    mov     gp,  zero
    mov     tp,  zero
    mov     t0,  zero
    mov     t1,  zero
    mov     t2,  zero
    mov     s0,  zero
    mov     s1,  zero
    mov     a0,  zero
    mov     a1,  zero
    mov     a2,  zero
    mov     a3,  zero
    mov     a4,  zero
    mov     a5,  zero
    mov     a6,  zero
    mov     a7,  zero
    mov     s2,  zero
    mov     s3,  zero
    mov     s4,  zero
    mov     s5,  zero
    mov     s6,  zero
    mov     s7,  zero
    mov     s8,  zero
    mov     s9,  zero
    mov     s10, zero
    mov     s11, zero
    mov     t3,  zero
    mov     t4,  zero
    mov     t5,  zero
    mov     t6,  zero
    
    la      sp, stack0
    li      a0, 1024*4
    csrr    a1, mhartid
    addi    a1, a1, 1
    mul     a0, a0, a1
    add     sp, sp, a0
    
    # jump to start() in start.c
    call    kernel_start

loop:
    j       . 
