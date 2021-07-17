/*
 *  SPDX-License-Identifier: GPLV2
 *  arch/csky/boot/boot.S
 *  X86 serial boot
 *  (C) 2020 wzl
 */

#include <asm-generic/header.h>
#include <asm/page.h>
#include <linkage.h>
 
    .section    .startup,"ax"

GLOBAL(boot_head)
    j       init                                /* 0x00: xtensa jmp  */
    .byte   0                                   /* 0x03: Reserved */
    .long   0, 0, 0                             /* 0x04: Reserved */
    KERNEL_HEADER
END(boot_head)
    
    .section    .init.text,"ax"
    .type       init, %function
    
ENTRY(init)
    
#ifdef CONFIG_XIP
Load_segment_Init:
    movi    a8, _ld_load_offset     /* segment lma */
    movi    a9, PAGE_OFFSET         /* segment vma */
    movi    a11, _ld_load_end       /* segment end */
    j       Load_loop
1:
    l32i    a12, a8, 0
    s32i    a12, a9, 0
    addi    a8, a8, 4
    addi    a9, a9, 4
Load_loop:
    bne     a9, a11, 1b
#endif

Bss_segment_Init:
    movi    a8, 0x00
    movi    a9, _ld_bss_start
    movi    a10, _ld_bss_end
    j       Bss_loop
1:
    s32i    a8, a9, 0
    addi    a9, a9, 4
Bss_loop:
    bne     a9, a10, 1b

env:
    movi    sp, init_thread_union + THREAD_SIZE

    call0   kernel_start
    j       halt
END(init)
  
ENTRY(halt)
    waiti   15
    j       .
END(halt)
