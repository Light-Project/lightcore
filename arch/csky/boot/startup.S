/*
 *  SPDX-License-Identifier: GPLV2
 *  arch/csky/boot/boot.S
 *  X86 serial boot
 *  (C) 2020 wzl
 */

#include <asm-generic/header.h>
#include <asm/page.h>
#include <asm/regs.h>
#include <linkage.h>
 
    .section    .startup,"ax"
GLOBAL(boot_head)
    lrw     r1, init                            /* 0x00: csky jmp  */
    jmp     r1
    .long   0, 0, 0                             /* 0x04: Reserved */
    KERNEL_HEADER
END(boot_head)
    
    .section    .init.text,"ax"
    .type       init, %function
ENTRY(init)
    xor     r1, r1
    lrw     r13, _ld_bss_start
    lrw     r14, _ld_bss_end
    jmpi    Bss_loop
1:
    st.w    r1, (r13, 0)
    addi    r13, 4
Bss_loop:
    cmphs   r13, r14
    bf      1b

    lrw     r1, init_thread_union + THREAD_SIZE
    mov     sp, r1

    jsri    kernel_start
END(init)
    
/* Stop the CPU and most peripherals */    
ENTRY(halt)
    stop
    br      halt
END(halt)
