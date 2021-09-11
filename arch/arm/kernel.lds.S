/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
* Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
*/

#include <asm-generic/kernel.lds.h>
#include <asm/page.h>
#include <asm/cache.h>

OUTPUT_ARCH(arm)
ENTRY(boot_head)

#define ARM_VECTOR(align)                       \
    . = ALIGN(align);                           \
    _ld_vectors_start = .;                      \
    .vectors 0xffff0000 :                       \
    AT(_ld_vectors_start) {                     \
        KEEP(*(.vectors))                       \
    }                                           \
    . = _ld_vectors_start + SIZEOF(.vectors);   \
    _ld_vectors_end = .;

#define ARM_UNWIND(align)                       \
    . = ALIGN(align);                           \
    .ARM.unwind_idx : {                         \
        __start_unwind_idx = .;                 \
        *(.ARM.exidx*)                          \
        __stop_unwind_idx = .;                  \
    }                                           \
    .ARM.unwind_tab : {                         \
        __start_unwind_tab = .;                 \
        *(.ARM.extab*)                          \
        __stop_unwind_tab = .;                  \
    }

SECTIONS
{
    . = CONFIG_PAGE_OFFSET;

    STARTUP_SECTION

    TEXT_SECTION(PAGE_SIZE)

    RODATA_SECTION(PAGE_SIZE)

    ARM_VECTOR(PAGE_SIZE)

#ifdef CONFIG_UNWIND
    ARM_UNWIND(4)
#endif

    RWDATA_SECTION(L1_CACHE_BYTES, PAGE_SIZE)

    INIT_TEXT_SECTION(PAGE_SIZE)

    INIT_DATA_SECTION(PAGE_SIZE)

    BSS_SECTION(L1_CACHE_BYTES, PAGE_SIZE, L1_CACHE_BYTES)

    _ld_image_end = .;

    DWARF_DEBUG
}

PROVIDE(_ld_image_size = _ld_image_end - _ld_startup_start);
