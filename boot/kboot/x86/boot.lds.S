/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
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

#define STACK_SIZE      (SZ_1KiB * 8)
#define HEAP_SIZE       (SZ_1KiB * 128)
#define KERNEL_OFFSET   0x100000

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

    .bootparam (NOLOAD) : {
        . = ALIGN(4);
        _ld_bootparam_start = .;
        *(.bootparam)
        *(.bootparam.*)
        _ld_bootparam_end = .;
    }

    . = ALIGN(512);
    _ld_header_end = .;

    . = KERNEL_OFFSET;

#undef LOAD_OFFSET
#define LOAD_OFFSET (KERNEL_OFFSET - _ld_header_end)

    _ld_kernel_start = .;

    .piggymagic : \
    AT(ADDR(.piggymagic) - LOAD_OFFSET) {
        _ld_piggy_magic = .;
        LONG(PIGGY_MAGIC)
    }

    .piggydata : \
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

    .stack (NOLOAD) : {
        . = ALIGN(4);
        _ld_stack_start = .;
        . = _ld_stack_start + STACK_SIZE;
        _ld_stack_end = .;
    }

    .heap (NOLOAD) : {
        . = ALIGN(4);
        _ld_heap_start = .;
        . = _ld_heap_start + HEAP_SIZE;
        . = ALIGN(4);
        _ld_heap_end = .;
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
PROVIDE(_ld_kernel_size = _ld_kernel_end - _ld_kernel_start);
PROVIDE(_ld_header_sects = (_ld_header_size / 512) - 1);
PROVIDE(_ld_sys_size = _ld_kernel_size / 16);
ASSERT(_ld_header_size < 0xfc00, "Boot head too big! (64KiB)");
