/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <linkage.h>
#include <asm/header.h>
#include "efi-header.S"

#define DEFAULT_MAJOR_ROOT  0
#define DEFAULT_MINOR_ROOT  0
#define DEFAULT_ROOT_DEV    (DEFAULT_MAJOR_ROOT << 8 | DEFAULT_MINOR_ROOT)

#define COMMAND_LINE_SIZE   2048

#define BOOTSEG         0x07C0  /* original address of boot-sector */
#define SYSSEG          0x1000  /* historical load address >> 4 */

#define NORMAL_VGA      0xffff  /* 80x25 mode */
#define EXTENDED_VGA    0xfffe  /* 80x50 mode */
#define ASK_VGA         0xfffd  /* ask for it at bootup */

#ifndef SVGA_MODE
# define SVGA_MODE NORMAL_VGA
#endif

#ifndef ROOT_RDONLY
# define ROOT_RDONLY 1
#endif

    .code16
    .section .head, "ax"

SYM_CODE_START(kboot_header)

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
sentinel:                   .byte       0xff, 0xff                  /* 0x1ef: Used to detect broken loaders */
kboot_params:                                                       /* linux header params: */
setup_sects:                .byte       _ld_header_sect             /* 0x1f1: Kboot header sectors */
root_flags:                 .word       ROOT_RDONLY                 /* 0x1f2: Rootfs flags */
syssize:                    .long       _ld_kernel_size             /* 0x1f4: Piggy data */
ram_size:                   .word       0                           /* 0x1f8: Obsolete */
vid_mode:                   .word       SVGA_MODE                   /* 0x1fa: Video mode */
root_dev:                   .word       DEFAULT_ROOT_DEV            /* 0x1fc: Default root device */
boot_flag:                  .word       0xaa55                      /* 0x1fe: MBR boot magic */
_start:                     .byte       0xeb                        /* 0x200: Offset 512, entry point */
_start2:                    .byte       selfcopy - header           /* 0x201: Short (2-byte) jump */
header:                     .ascii      "HdrS"                      /* 0x202: Header signature */
version:                    .word       0x020f                      /* 0x206: Header version number */
realmode_swtch:             .word       0, 0                        /* 0x208: default_switch, SETUPSEG */
start_sys_seg:              .word       SYSSEG                      /* 0x20c: Obsolete */
kernel_version:             .word       kernel_version_msg - 512    /* 0x20e: Pointing to kernel version string */
type_of_loader:             .byte       0                           /* 0x210: 0 means ancient bootloader */
loadflags:                  .byte       LOADED_HIGH                 /* 0x211: The kernel is to be loaded high */
setup_move_size:            .word       0x8000                      /* 0x212: size to move, when setup is not, loaded at 0x90000 */
code32_start:               .long       0x100000                    /* 0x214: Start address for 32-bit code */
ramdisk_image:              .long       0                           /* 0x218: Address of loaded ramdisk image */
ramdisk_size:               .long       0                           /* 0x21c: Ramdisk size in bytes */
bootsect_kludge:            .long       0                           /* 0x220: Obsolete */
heap_end_ptr:               .word       0                           /* 0x224:  */
ext_loader_ver:             .byte       0                           /* 0x226: Extended boot loader version */
ext_loader_type:            .byte       0                           /* 0x227: Extended boot loader type */
cmd_line_ptr:               .long       0                           /* 0x228: Pointer to the kernel command line */
initrd_addr_max:            .long       0x7fffffff                  /* 0x22c: The highest safe address for the contents of an initrd */
kernel_alignment:           .long       0x200000                    /* 0x230: Physical addr alignment */
relocatable_kernel:         .byte       0                           /* 0x234: Relocatable kernel */
min_alignment:              .byte       0                           /* 0x235: Minimum alignment */
xloadflags:                 .word       0                           /* 0x236:  */
cmdline_size:               .long       COMMAND_LINE_SIZE - 1       /* 0x238: Length of the command line */
hardware_subarch:           .long       0                           /* 0x23c: Subarchitecture, 0 for normal x86 PC */
hardware_subarch_data:      .quad       0                           /* 0x240: Subarchitecture data */
payload_offset:             .long       0                           /* 0x248:  */
payload_length:             .long       0                           /* 0x24c:  */
setup_data:                 .quad       0                           /* 0x250: Single linked list of struct setup_data */
pref_address:               .quad       0                           /* 0x258: Preferred load addr */
init_size:                  .long       0                           /* 0x260: Kernel initialization size */
handover_offset:            .long       0                           /* 0x264: EFI stub entry */
kernel_info_offset:         .long       0                           /* 0x268: Kernel info offset */
kernel_version_msg:         .ascii      CONFIG_VERSION

SYM_CODE_END(kboot_header)

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
