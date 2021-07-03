/* SPDX-License-Identifier: GPL-2.0-or-later */
#include <asm-generic/kernel.lds.h>
#include <asm/page.h>

OUTPUT_ARCH(i386)
ENTRY(startup_head)

SECTIONS
{
    . = CONFIG_PAGE_OFFSET;
    
    . += 0x1000000;
    
    STARTUP_SECTION
    
    TEXT_SECTION(PAGE_SIZE)
    
    RODATA_SECTION(PAGE_SIZE)
    
    RWDATA_SECTION(L1_CACHE_BYTES, PAGE_SIZE, THREAD_SIZE)
    
    INIT_TEXT_SECTION(PAGE_SIZE)
    
    INIT_DATA_SECTION(PAGE_SIZE)
    
    PROVIDE(_ld_kernel_size = . - CONFIG_PAGE_OFFSET);

    BSS_SECTION(PAGE_SIZE, PAGE_SIZE, PAGE_SIZE)
    
    DWARF_DEBUG
    
    /* Sections to be discarded */
    DISCARDS
}

