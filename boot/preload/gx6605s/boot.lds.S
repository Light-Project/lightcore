/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 John Sanpe <sanpeqf@gmail.com>
 */

OUTPUT_ARCH(csky)
ENTRY(_start)

MEMORY {
    SIG  (r)  : ORIGIN = 0x800ffffc,  LENGTH = 0x0004
    SPL  (rwx): ORIGIN = 0x80100000,  LENGTH = 0x2000
    SRAM (rwx): ORIGIN = 0x80102000,  LENGTH = 0x2000
}

SECTIONS {
    .signature : {
        LONG(0x55aa55aa)
    } > SIG

    .startup : {
        . = ALIGN(4);
        *(.startup)
    } > SPL

    .text : {
        . = ALIGN(4);
        *(.text)
        *(.text*)
    } > SPL

    .rodata : {
        *(.rodata)
        *(.rodata*)
    } > SPL

    .data : {
        *(.data)
        *(.data*)
    } > SPL

    .bss (NOLOAD) : {
        . = ALIGN(4);
        _ld_bss_start = .;
        *(.bss)
        *(.bss*)
        . = ALIGN(4);
        _ld_bss_end = .;
    } > SRAM

    .stack (NOLOAD) : {
        . = ALIGN(4);
        _ld_stack_start = .;
        . = _ld_stack_start + 0x400;
        _ld_stack_end = .;
    } > SRAM
}
