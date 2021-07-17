#include <asm-generic/kernel.lds.h>
#include <asm/page.h>

OUTPUT_ARCH(arm)
ENTRY(boot_head)

SECTIONS
{
    . = CONFIG_PAGE_OFFSET;
    
    STARTUP_SECTION
    
    TEXT_SECTION(PAGE_SIZE)
    
    RODATA_SECTION(PAGE_SIZE)
    
    RWDATA_SECTION(L1_CACHE_BYTES, PAGE_SIZE, THREAD_SIZE)
    
    INIT_TEXT_SECTION(PAGE_SIZE)
    
    INIT_DATA_SECTION(PAGE_SIZE)
    
    PROVIDE(_ld_kernel_size = . - _ld_sstartup);

    BSS_SECTION(PAGE_SIZE, PAGE_SIZE, PAGE_SIZE)
    
    DWARF_DEBUG
    
    DISCARDS
}

