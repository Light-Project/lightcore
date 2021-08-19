#include <asm-generic/kernel.lds.h>
#include <asm/page.h>
#include <asm/cache.h>

OUTPUT_ARCH(arm)
ENTRY(boot_head)

SECTIONS
{
    . = CONFIG_PAGE_OFFSET;

    STARTUP_SECTION

    TEXT_SECTION(PAGE_SIZE)

    RODATA_SECTION(PAGE_SIZE)

    RWDATA_SECTION(L1_CACHE_BYTES, PAGE_SIZE)

    INIT_TEXT_SECTION(PAGE_SIZE)

    INIT_DATA_SECTION(PAGE_SIZE)

    BSS_SECTION(L1_CACHE_BYTES, PAGE_SIZE, L1_CACHE_BYTES)

    _ld_image_end = .;

    DWARF_DEBUG

    /* Sections to be discarded */
    DISCARDS
}
PROVIDE(_ld_image_size = _ld_image_end - _ld_startup_start);
