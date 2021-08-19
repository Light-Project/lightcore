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

#include <linkage.h>

    .arm
    .section    .setup,"ax"

GLOBAL(_start)
    ldr pc, =init                                   /* 0x00: jump instruction   */
    .byte 'e', 'G', 'O', 'N', '.', 'B', 'T', '0'    /* 0x04: magic              */
    .long 0                                         /* 0x0c: crc32 checksum     */
    .long _ld_spl_size                              /* 0x10: Spl length         */
    .byte 'S', 'P', 'L', 2                          /* 0x14: Spl signature      */
    .long 0                                         /* 0x18: fel script address */
    .long 0                                         /* 0x1c: fel uenv length    */
    .long 0, 0, 0, 0                                /* 0x20: dt name offset     */
    .long 0, 0, 0, 0
    .long 0, 0, 0, 0
    .long 0, 0, 0, 0
END(_start)

ENTRY(init)
    mrs     r0, cpsr
    stmfd   sp!, {r0, lr}

    movs    r0, #0
    ldr     r1, =_ld_sbss
    ldr     r2, =_ld_ebss
    b       Bss_loop
1:
    str     r0, [r1], #4
Bss_loop:
    cmp     r1, r2
    bcc     1b

    bl      main

    ldmfd   sp!, {r0, lr}
    msr     cpsr, r0
    bx      lr
END(init)

GLOBAL(kboot_start)
    mov     pc, a1
END(kboot_start)

GLOBAL(halt)
    bkpt
    b   halt
END(halt)

