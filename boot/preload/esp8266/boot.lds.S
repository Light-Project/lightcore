/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <asm-generic/kernel.lds.h>
#include <romcall.ld>
#include <size.h>

#define stack_size (SZ_1KiB * 4)

OUTPUT_ARCH(xtensa)

/* Entry Point */
ENTRY(_start)

MEMORY {
    FLASH (rx)  : ORIGIN = 0x40200000, LENGTH = CONFIG_XIP_BASE - 0x40200000
    IRAM  (rwx) : ORIGIN = 0x40100000, LENGTH = 32k
    DRAM0 (rw)  : ORIGIN = 0x3FFE8000, LENGTH = 64K
    DRAM1 (rw)  : ORIGIN = 0x3FFF8000, LENGTH = 32K
}

SECTIONS {
    .text : {
        /* Boot head */
        . = 0x00;
        KEEP(*(.startup));

        /* Vectors start */
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
        *(.ksymtab)
    } > IRAM AT > FLASH

    .data : {
        . = ALIGN(4);
        *(.rodata)
        *(.rodata*)
        *(.data)
        *(.data*)
        . = ALIGN(4);
    } > DRAM0 AT > FLASH

    .bss (NOLOAD) : {
        . = ALIGN(4);
        PROVIDE(_ld_sbss = .);
        *(.bss)
        *(.bss*)
        . = ALIGN(4);
        PROVIDE(_ld_ebss = .);
    } > DRAM0

    .stack(NOLOAD) : {
        . = ALIGN(4);
        _ld_stack_start = .;
        . = _ld_stack_start + stack_size;
        . = ALIGN(4);
        _ld_stack_end = .;
    } > DRAM0

    .dts : {
        . = ALIGN(4);
        PROVIDE(_ld_dts_start = .);
        KEEP(*(.dts))
        . = ALIGN(4);
    } > DRAM1 AT > FLASH

    DWARF_DEBUG

    /* Sections to be discarded */
    DISCARDS
}
