/* SPDX-License-Identifier: GPL-2.0-or-later */
#include <asm-generic/kernel.lds.h>
#include <asm/cache.h>
#include <asm/page.h>

OUTPUT_ARCH(csky)
ENTRY(boot_head)

SECTIONS
{
    . = pa_to_va(NORMAL_OFFSET);

    STARTUP_SECTION

    TEXT_SECTION(PAGE_SIZE)

    RODATA_SECTION(PAGE_SIZE)

    RWDATA_SECTION(CACHE_LINE_SIZE, PAGE_SIZE)

    INIT_TEXT_SECTION(PAGE_SIZE)

    INIT_DATA_SECTION(PAGE_SIZE)

    EXIT_TEXT_SECTION(PAGE_SIZE)

    EXIT_DATA_SECTION(PAGE_SIZE)

    BSS_SECTION(CACHE_LINE_SIZE, PAGE_SIZE, CACHE_LINE_SIZE)

    _ld_image_end = .;

    DWARF_DEBUG

    /* Sections to be discarded */
    DISCARDS
}
PROVIDE(_ld_image_size = _ld_image_end - _ld_startup_start);
