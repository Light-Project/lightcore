/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

OUTPUT_ARCH(arm)
ENTRY(_start)

SECTIONS {
    .setup : {
        . = ALIGN(4);
        *(.setup)
    }

    .text : {
        . = ALIGN(4);
        *(.text)
        *(.text*)
    }

    .rodata : {
        . = ALIGN(4);
        *(.rodata)
        *(.rodata*)
    }

    .data : {
        . = ALIGN(4);
        *(.data)
        *(.data*)
    }

    PROVIDE(_ld_spl_size = . - 0);

    .bss (NOLOAD) : {
        . = ALIGN(4);
        _ld_sbss = .;
        *(.bss)
        *(.bss*)
        . = ALIGN(4);
        _ld_ebss = .;
    }
}
