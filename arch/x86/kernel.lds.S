/* SPDX-License-Identifier: GPL-2.0-or-later */
#include <asm-generic/kernel.lds.h>
#include <asm/page.h>

#if defined(CONFIG_ARCH_X86)
OUTPUT_ARCH(i386)
#elif defined(CONFIG_ARCH_X86_64)
OUTPUT_ARCH(i386:x86-64)
#else
# error "unknown instruction set"
#endif

ENTRY(startup_head)

SECTIONS {
    . = pa_to_va(NORMAL_OFFSET);

    STARTUP_SECTION

    TEXT_SECTION(PAGE_SIZE)

    RODATA_SECTION(PAGE_SIZE)

    RWDATA_SECTION(CACHE_LINE_SIZE, PAGE_SIZE)

    INIT_TEXT_SECTION(PAGE_SIZE)

    INIT_DATA_SECTION(PAGE_SIZE)

    PROVIDE(_ld_kernel_size = . - CONFIG_PAGE_OFFSET);

    BSS_SECTION(PAGE_SIZE, PAGE_SIZE, PAGE_SIZE)

    _ld_image_end = .;

    DWARF_DEBUG

    /* Sections to be discarded */
    DISCARDS
}

PROVIDE(_ld_image_size = _ld_image_end - _ld_startup_start);
