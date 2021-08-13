/* SPDX-License-Identifier: GPL-2.0-or-later */
#include <asm-generic/kernel.lds.h>
#include <asm/page.h>

OUTPUT_ARCH(i386)
ENTRY(startup_head)

SECTIONS
{
#ifdef CONFIG_MMU
    . = CONFIG_PAGE_OFFSET;
#endif
#ifdef CONFIG_REGION_DMA
    . += CONFIG_DMA_SIZE;
#endif

    STARTUP_SECTION

    TEXT_SECTION(PAGE_SIZE)

    RODATA_SECTION(PAGE_SIZE)

    RWDATA_SECTION(L1_CACHE_BYTES, PAGE_SIZE)

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
