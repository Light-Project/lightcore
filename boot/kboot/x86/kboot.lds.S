/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <size.h>

OUTPUT_FORMAT("elf32-i386")
OUTPUT_ARCH("i386")
ENTRY(_start)

#define stack_size  (SZ_1KiB * 8)
#define heap_size   (SZ_1KiB * 64)

SECTIONS {
    . = 0;
    _ld_header_start = .;

    .head : {
        KEEP(*(.head))
        KEEP(*(.head.*))
    }

    .startup : {
        KEEP(*(.startup))
        KEEP(*(.startup.*))
    }

    _ld_startup_end = .;

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

    .got : {
        . = ALIGN(4);
        PROVIDE(_ld_got_start = .);
        *(.got)
        *(.got*)
        . = ALIGN(4);
        PROVIDE(_ld_got_end = .);
    }

    _ld_header_end = .;

    .piggydata : {
        _ld_piggy_start = .;
        KEEP(*(.piggydata))
        _ld_piggy_end = .;
    }

    .align : {
        PROVIDE(_ld_align = .);
        SHORT(0xaa55)
        . = ALIGN(512);
    }

    _ld_kernel_end = .;

    .bss (NOLOAD) : {
        . = ALIGN(4);
        _ld_bss_start = .;
        *(.bss)
        *(.bss*)
        . = ALIGN(4);
        _ld_bss_end = .;
    }

    .stack (NOLOAD) : {
        . = ALIGN(4);
        _ld_stack_start = .;
        . = _ld_stack_start + stack_size;
        _ld_stack_end = .;
    }

    .heap (NOLOAD) : {
        . = ALIGN(4);
        _ld_heap_start = .;
        . = _ld_heap_start + heap_size;
        . = ALIGN(4);
        _ld_heap_end = .;
    }

    /DISCARD/ : {
        *(.note*)
    }

    PROVIDE(_ld_header_size = _ld_header_end - _ld_header_start);
    PROVIDE(_ld_kernel_size = _ld_kernel_end - _ld_header_end);
    PROVIDE(_ld_header_sects = _ld_header_size / 512);
    PROVIDE(_ld_kernel_sects = _ld_kernel_size / 512);
}
