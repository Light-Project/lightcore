#include <asm-generic/kernel.lds.h>
#include <asm/page.h>
#include <asm/cache.h>

OUTPUT_ARCH(csky)
ENTRY(boot_head)

SECTIONS
{
    . = CONFIG_PAGE_OFFSET + CONFIG_RAM_BASE;
    
    STARTUP_SECTION
    
    TEXT_SECTION(PAGE_SIZE)
    
    RODATA_SECTION(PAGE_SIZE)
    
    RWDATA_SECTION(L1_CACHE_BYTES, PAGE_SIZE, THREAD_SIZE)
    
    INIT_TEXT_SECTION(PAGE_SIZE)
    
    INIT_DATA_SECTION(PAGE_SIZE)
    
    PROVIDE(_ld_kernel_size = . - _ld_text_start);

    BSS_SECTION(L1_CACHE_BYTES, PAGE_SIZE, L1_CACHE_BYTES)
    
    _ld_image_end = .;
    
    DWARF_DEBUG
    
    /* Sections to be discarded */
    DISCARDS
}

