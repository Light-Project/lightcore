/* SPDX-License-Identifier: GPL-2.0-or-later */
#include <size.h>

#define stack_size (size_1KiB * 8)
#define heap_size (size_1KiB * 64)

ENTRY(_start)

SECTIONS
{
    . = CONFIG_LOAD_ADDR;
    head_start = .;

    .head : {
        KEEP(*(.head))
    }

    .startup : {
        KEEP(*(.startup))
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

    _ld_end = .;

    .bss(NOLOAD) : {
        . = ALIGN(4);
        _ld_bss_start = .;
        *(.bss)
        *(.bss*)
        . = ALIGN(4);
        _ld_bss_end = .;
    }

    .stack(NOLOAD) : {
        . = ALIGN(4);
        _ld_stack_start = .;
        . = _ld_stack_start + stack_size;
        _ld_stack_end = .;
    }

    .heap(NOLOAD) : {
        . = ALIGN(4);
        _ld_heap_start = .;
        . = _ld_heap_start + heap_size;
        . = ALIGN(4);
        _ld_heap_end = .;
    }
    
    /DISCARD/ : {
        *(.note*)
    }

    PROVIDE(_ld_size = _ld_end - head_start);
    PROVIDE(_ld_bsize = _ld_size / 512);
}
