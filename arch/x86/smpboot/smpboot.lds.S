/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

OUTPUT_FORMAT("elf32-i386")
OUTPUT_ARCH(i386)
ENTRY(_start)

SECTIONS {
    .setup : {
        PROVIDE(_ld_setup_start = .);
        KEEP(*(.setup))
        PROVIDE(_ld_esetup = .);
    }

    .text : {
        . = ALIGN(4);
        PROVIDE(_ld_text_start = .);
        *(.text)
        *(.text*)
        . = ALIGN(4);
        PROVIDE(_ld_text_end = .);
    }

    .rodata : {
        PROVIDE(_ld_rodata_start = .);
        *(.rodata)
        *(.rodata*)
        PROVIDE(_ld_rodata_end = .);
    }

    .data : {
        PROVIDE(_ld_data_start = .);
        *(.data)
        *(.data*)
        PROVIDE(_ld_data_end = .);
    }

    .align : {
        PROVIDE(_ld_align_start = .);
        SHORT(0xaa55)
        . = ALIGN(512);
        PROVIDE(_ld_align_end = .);
    }

    .bss (NOLOAD) : {
        PROVIDE(_ld_bss_start = .);
        *(.bss)
        *(.bss*)
        PROVIDE(_ld_bss_end = .);
    }

    .stack (NOLOAD) : {
        . = ALIGN(4);
        PROVIDE(_ld_stack_start = .);
        . = _ld_stack_start + 0x1000;
        . = ALIGN(4);
        PROVIDE(_ld_stack_end = .);
    }
}
