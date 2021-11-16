/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <linkage.h>
#include "efi-header.S"

BOOTSEG     = 0x07C0        /* original address of boot-sector */
SYSSEG      = 0x1000        /* historical load address >> 4 */

#define ASK_VGA		0xfffd		/* ask for it at bootup */
#ifndef SVGA_MODE
#define SVGA_MODE ASK_VGA
#endif

#ifndef ROOT_RDONLY
#define ROOT_RDONLY 1
#endif

    .code16
    .section .head,"ax"
GLOBAL(head)

#ifdef CONFIG_EFI_STUB
    .word    MZ_MAGIC
#endif
    /* Normalize the start address */
    ljmp    $BOOTSEG, $start2
start2:
    movw    %cs, %ax
    movw    %ax, %ds
    movw    %ax, %es
    movw    %ax, %ss
    xorw    %sp, %sp

    sti
    cld

    movb    $0xe, %ah
    movw    $7, %bx
    movw    $bugger_off_msg, %si
1:
    lodsb
    andb    %al, %al
    jz      reboot
    int     $0x10
    jmp     1b

reboot:
    /* Allow the user to press a key, then reboot */
    xorw    %ax, %ax
    int     $0x16
    int     $0x19

    /*
     * int 0x19 should never return.  In case it does anyway,
     * invoke the BIOS reset code...
     */
    ljmp    $0xf000,$0xfff0

#ifdef CONFIG_EFI_STUB
    .org    head + 0x3c
    /* Offset to the PE header */
    .long   pe_header
#endif

bugger_off_msg:
    .ascii  "Use a boot loader.\n\r"
    .ascii  "Remove disk and press any key to reboot...\n\r"
    .ascii  "\n\r"
    .byte   0

#ifdef CONFIG_EFI_STUB
    EFI_HEADER
#endif

    .org        head + 0x1ef
sentinel:       .byte 0xff, 0xff
setup_sects:    .byte 0
root_flags:     .word ROOT_RDONLY
syssize:        .long _ld_bsize
ram_size:       .word 0
vid_mode:       .word SVGA_MODE
root_dev:       .word 0
boot_flag:      .word 0xaa55

END(head)

GLOBAL(_start)
    # Explicitly enter this as bytes, or the assembler
    # tries to generate a 3-byte jump here, which causes
    # everything else to push off to the wrong offset.
    .byte    0xeb       # short (2-byte) jump
    .byte    startup16 - 1f
1:
END(_start)
