/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 John Sanpe <sanpeqf@gmail.com>
 */

#include <size.h>
#include <kboot.h>
#include <asm-generic/kernel.lds.h>

#if defined(CONFIG_ARCH_X86_32)
OUTPUT_ARCH(i386)
#elif defined(CONFIG_ARCH_X86_64)
OUTPUT_ARCH(i386:x86-64)
#else
# error "unknown instruction set"
#endif

ENTRY(_start)

#define STACK_SIZE      (SZ_1KiB * 32)
#define HEAP_SIZE       (SZ_1KiB * 128)
#define KERNEL_OFFSET   0x100000

SECTIONS {
/*
 * TODO: It should be 0x500 here, but I don't
 * know how to align the head size.
 */
    . = 0x600;
    _ld_header_start = .;

    .head : {
        KEEP(*(.head))
        KEEP(*(.head.*))
    }

    .startup : {
        KEEP(*(.startup))
        KEEP(*(.startup.*))
    }

    .text : {
        . = ALIGN(4);
        *(.text)
        *(.text.*)
    }

    .rodata : {
        . = ALIGN(4);
        *(.rodata)
        *(.rodata.*)
    }

    .data : {
        . = ALIGN(4);
        *(.data)
        *(.data.*)
    }

    . = ALIGN(512);
    _ld_header_end = .;

    .bootparam (NOLOAD) : {
        . = ALIGN(4);
        _ld_bootparam_start = .;
        *(.bootparam)
        *(.bootparam.*)
        _ld_bootparam_end = .;
    }

    . = ALIGN(4);
    _ld_memory_end = .;

    . = KERNEL_OFFSET;

#undef LOAD_OFFSET
#define LOAD_OFFSET (KERNEL_OFFSET - _ld_header_end)

    _ld_kernel_start = .;

    .piggymagic :
    AT(ADDR(.piggymagic) - LOAD_OFFSET) {
        _ld_piggy_magic = .;
        LONG(PIGGY_MAGIC)
    }

    .piggydata :
    AT(ADDR(.piggydata) - LOAD_OFFSET) {
        _ld_piggy_start = .;
        KEEP(*(.piggydata))
        _ld_piggy_end = .;
    }

    . = ALIGN(512);
    _ld_kernel_end = .;

#undef LOAD_OFFSET

    .bss (NOLOAD) : {
        . = ALIGN(4);
        _ld_bss_start = .;
        *(.bss)
        *(.bss.*)
        *(COMMON)
        . = ALIGN(4);
        _ld_bss_end = .;
    }

    .heap (NOLOAD) : {
        . = ALIGN(4);
        _ld_heap_start = .;
        . = _ld_heap_start + HEAP_SIZE;
        . = ALIGN(4);
        _ld_heap_end = .;
    }

    .stack (NOLOAD) : {
        . = ALIGN(4);
        _ld_stack_start = .;
        . = _ld_stack_start + STACK_SIZE;
        _ld_stack_end = .;
    }

    STABS_DEBUG
    DWARF_DEBUG
    ELF_DETAILS
    DISCARDS

    /DISCARD/ : {
        *(.dynamic) *(.dynsym) *(.dynstr) *(.dynbss)
        *(.hash) *(.gnu.hash)
        *(.note.*)
    }
}

PROVIDE(_ld_header_size = _ld_header_end - _ld_header_start);
PROVIDE(_ld_memory_size = _ld_memory_end - _ld_header_start);
PROVIDE(_ld_kernel_size = _ld_kernel_end - _ld_kernel_start);
PROVIDE(_ld_header_sect = _ld_header_size / 512 - 1);
PROVIDE(_ld_kernel_size = _ld_kernel_size / 16);
ASSERT(_ld_memory_size < 0xfa00, "boot memory too big! (62.5KiB)");
