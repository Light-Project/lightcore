/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 John Sanpe <sanpeqf@gmail.com>
 */

#include <linkage.h>

    .section    .startup,"ax"
GLOBAL(_start)
    br          init
    br          init
    .long       misaligned_access               /* 0x04  Misaligned Access (The bootloader valid flag for nationalchip bootrom) */
    .long       access_error                    /* 0x08  Access Error                                                           */
    .long       divide_by_zero                  /* 0x0C  Divided by Zero                                                        */
    .long       illegal_instruction             /* 0x10  Illegal Instruction                                                    */
    .long       privilege_violation             /* 0x14  Privilege Violation                                                    */
    .long       trace_exception                 /* 0x18  Trace Exception                                                        */
    .long       breakpoint_exception            /* 0x1C  Breakpoint Exception                                                   */
    .long       unrecoverable_error             /* 0x20  Unrecoverable Error                                                    */
    .long       soft_reset                      /* 0x24  Soft Reset                                                             */
    .long       normal_autovector_interrupt     /* 0x28  Normal Autovector Interrupt                                            */
    .long       fast_autovector_interrupt + 1   /* 0x2C  Fast Interrupt Autovector (AF used)                                    */
    .long       hardware_accelerator            /* 0x30  Hardware Accelerator                                                   */
    .space      0x0c                            /*       Unused exception space                                                 */
END(_start)

ENTRY(init)
    psrclr      ee, fe, ie
    psrclr      af

    /* flush v4 I/D caches */
    mfcr        r1, cr17
    bseti       r1, 0
    bseti       r1, 1
    bseti       r1, 4
    mtcr        r1, cr17

    /* setup caches register */
    mfcr        r1, cr18
    bseti       r1, 2
    bclri       r1, 3
    bseti       r1, 5
    bseti       r1, 6
    mtcr        r1, cr18

    /* nop cycles for stability (not necessary) */
    mov         r0, r0
    mov         r0, r0
    mov         r0, r0
    mov         r0, r0

    /* invalid tlb before setup mmu */
    cpseti      cp15
    bgeni       r1, 27
    cpwcr       r1, cpcr8
    bgeni       r1, 26
    cpwcr       r1, cpcr8

    /* set mmu pgd address */
    lrw         r1, 0
    cpwcr       r1, cpcr5

    /* set page size (1MiB) */
    lrw         r1, 0xffff << 13
    cpwcr       r1, cpcr6

    /* set virtual start address */
    lrw         r1, 0x00
    cpwcr       r1, cpcr4

    /* set page attribute and coresponding physical addresses */
    lrw         r1, (0x000000 << 6) | 0x1f  /* cache able */
    cpwcr       r1, cpcr2
    lrw         r1, (0x100000 << 6) | 0x1f  /* cache able */
    cpwcr       r1, cpcr3

    /* stuffing data to jtlb */
    bgeni       r1, 28
    cpwcr       r1, cpcr8
    movi        r1, 0x0
    cpwcr       r1, cpcr8

    /* setup segment registers */
    lrw         r1, 0x00000000 | 0x0e   /* cache able */
    cpwcr       r1, cpcr30
    lrw         r1, 0x00000000 | 0x06   /* cache unable */
    cpwcr       r1, cpcr31

    /* enable pld and mb */
    mfcr        r1, cr30
    bseti       r1, 2
    bseti       r1, 3
    mtcr        r1, cr30

    /* enable mmu and cache */
    mfcr        r1, cr18
    bseti       r1, 0
    bclri       r1, 1
    bseti       r1, 2
    bseti       r1, 3
    bseti       r1, 4
    bseti       r1, 5
    bseti       r1, 6
    mtcr        r1, cr18

    /* normalize the memory address */
    lrw         r1, env
    jmp         r1
END(init)

ENTRY(env)
    /* initialize the Vector Base Register */
    lrw         r1, _start
    mtcr        r1, vbr

    /* clear the bss region */
    xor         r1, r1
    lrw         l0, _ld_bss_start
    lrw         l1, _ld_bss_end
    br          Bss_loop
1:
    st.w        r1, (l0, 0)
    addi        l0, 4
Bss_loop:
    cmphs       l0, l1
    bf          1b

    /* set stack pointer */
    lrw         r1, _ld_stack_end
    mov         sp, r1

    psrset      ee, fe, ie
    bsr         main
END(env)

GLOBAL(kboot_start)
    mov         r1, a0
    lrw         a0, 0x4c434747  /* arg1: boot magic   */
    lrw         a1, 0x00000000  /* arg2: dtb start    */
    lrw         a2, uart_print  /* arg3: kboot stdout */
    jmp         r1
END(kboot_start)

GLOBAL(halt)
    stop
    br          halt
END(halt)

misaligned_access:
access_error:
divide_by_zero:
illegal_instruction:
privilege_violation:
trace_exception:
breakpoint_exception:
unrecoverable_error:
soft_reset:
normal_autovector_interrupt:
fast_autovector_interrupt:
hardware_accelerator:
    /* get exception vector */
    mfcr        a0, psr
    lsli        a0, 9
    lsri        a0, 25
    jmpi        exception_handle
