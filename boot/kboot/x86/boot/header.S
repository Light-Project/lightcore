/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <linkage.h>
#include "efi-header.S"

#define BOOTSEG             0x07C0  /* original address of boot-sector */
#define SYSSEG              0x1000  /* historical load address >> 4 */

#define NORMAL_VGA          0xffff  /* 80x25 mode */
#define EXTENDED_VGA        0xfffe  /* 80x50 mode */
#define ASK_VGA             0xfffd  /* ask for it at bootup */

#define COMMAND_LINE_SIZE   2048

#ifndef SVGA_MODE
# define SVGA_MODE NORMAL_VGA
#endif

#ifndef ROOT_RDONLY
# define ROOT_RDONLY 1
#endif

    .code16
    .section .head, "ax"
GLOBAL(kboot_header)

#ifdef CONFIG_EFI_STUB
    .word    MZ_MAGIC
#endif

    /* Normalize the start address */
    ljmp    $BOOTSEG, $1f
1:

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

    .org kboot_header + 0x1ef
sentinel:                   .byte       0xff, 0xff                  /*  */
kboot_params:
setup_sects:                .byte       _ld_header_sects            /*  */
root_flags:                 .word       ROOT_RDONLY                 /*  */
syssize:                    .long       _ld_kernel_size             /* Obsolete */
ram_size:                   .word       0                           /*  */
vid_mode:                   .word       SVGA_MODE                   /*  */
root_dev:                   .word       0                           /*  */
boot_flag:                  .word       0xaa55                      /*  */
_start:                     .byte       0xeb                        /*  */
                            .byte       startup16 - header
header:                     .ascii      "HdrS"                      /* Header signature */
version:                    .word       0x020f                      /* Header version number */
realmode_swtch:             .word       0, 0                        /* default_switch, SETUPSEG */
start_sys_seg:              .word       SYSSEG                      /* Obsolete */
kernel_version:             .word       kernel_version_msg - 512    /* Pointing to kernel version string */
type_of_loader:             .byte       0                           /*  */
loadflags:                  .byte       0                           /* The kernel is to be loaded high */
setup_move_size:            .word       0x8000                      /*  */
code32_start:               .long       0x100000                    /* Start address for 32-bit code */
ramdisk_image:              .long       0                           /* Address of loaded ramdisk image */
ramdisk_size:               .long       0                           /* Ramdisk size in bytes */
bootsect_kludge:            .long       0                           /* Obsolete */
heap_end_ptr:               .word       0                           /*  */
ext_loader_ver:             .byte       0                           /* Extended boot loader version */
ext_loader_type:            .byte       0                           /* Extended boot loader type */
cmd_line_ptr:               .long       0                           /*  */
initrd_addr_max:            .long       0x7fffffff                  /*  */
kernel_alignment:           .long       0                           /*  */
min_alignment:              .byte       0                           /*  */
relocatable_kernel:         .byte       0                           /*  */
xloadflags:                 .word       0                           /*  */
cmdline_size:               .long       COMMAND_LINE_SIZE - 1       /* Length of the command line */
hardware_subarch:           .long       0                           /* Subarchitecture, 0 for normal x86 PC */
hardware_subarch_data:      .quad       0                           /*  */
payload_offset:             .long       0                           /*  */
payload_length:             .long       0                           /*  */
setup_data:                 .quad       0                           /*  */
pref_address:               .quad       0                           /* Preferred load addr */
init_size:                  .long       0                           /* Kernel initialization size */
handover_offset:            .long       0                           /*  */
kernel_info_offset:         .long       0                           /*  */
kernel_version_msg:         .ascii      CONFIG_VERSION
END(kboot_header)

.global sentinel
.global kboot_params
.global setup_sects
.global root_flags
.global syssize
.global ram_size
.global vid_mode
.global root_dev
.global boot_flag
.global _start
.global header
.global version
.global realmode_swtch
.global start_sys_seg
.global kernel_version
.global type_of_loader
.global loadflags
.global setup_move_size
.global code32_start
.global ramdisk_image
.global ramdisk_size
.global bootsect_kludge
.global heap_end_ptr
.global ext_loader_ver
.global ext_loader_type
.global cmd_line_ptr
.global initrd_addr_max
.global kernel_alignment
.global min_alignment
.global relocatable_kernel
.global xloadflags
.global cmdline_size
.global hardware_subarch
.global hardware_subarch_data
.global payload_offset
.global payload_length
.global setup_data
.global pref_address
.global init_size
.global handover_offset
.global kernel_info_offset
