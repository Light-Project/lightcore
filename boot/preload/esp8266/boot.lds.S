/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <romcall.ld>
#include <size.h>

#define stack_size (SZ_1KiB * 4)

OUTPUT_ARCH(xtensa)
ENTRY(_start)

MEMORY {
    FLASH (rx)  : ORIGIN = 0x40200000, LENGTH = 32k
    IRAM  (rwx) : ORIGIN = 0x40100000, LENGTH = 32k
    DRAM0 (rw)  : ORIGIN = 0x3FFE8000, LENGTH = 64K
    DRAM1 (rw)  : ORIGIN = 0x3FFF8000, LENGTH = 32K
}

SECTIONS {
    .text : {
        . = 0x00;
        KEEP(*(.startup));
        . = 0x10;
        KEEP(*(.Debug.Vector));
        . = 0x20;
        KEEP(*(.NMI.Vector));
        . = 0x30;
        KEEP(*(.Kernel.Vector));
        . = 0x50;
        KEEP(*(.User.Vector));
        . = 0x70;
        KEEP(*(.Double.Vector));
        . = 0x80;
        KEEP(*(.Reset.Vector));
        . = 0x90;
        *(*.Vector.literal)

        *(.literal)
        *(.text)
        *(.literal*)
        *(.text*)
    } > IRAM AT > FLASH

    .rodata : {
        . = ALIGN(4);
        *(.rodata)
        *(.rodata*)
    } > DRAM0 AT > FLASH

    .data : {
        . = ALIGN(4);
        *(.data)
        *(.data*)
    } > DRAM0 AT > FLASH

    .bss (NOLOAD) : {
        . = ALIGN(4);
        _ld_bss_start = .;
        *(.bss)
        *(.bss*)
        . = ALIGN(4);
        _ld_bss_end = .;
    } > DRAM0

    .stack(NOLOAD) : {
        . = ALIGN(4);
        _ld_stack_start = .;
        . = _ld_stack_start + stack_size;
        _ld_stack_end = .;
    } > DRAM0
}
