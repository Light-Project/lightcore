#include <asm-generic/kernel.lds.h>
#include <asm/page.h>
#include "romcall.lds.h"

OUTPUT_ARCH(arm)

/* Entry Point */
ENTRY(boot_head)

/* Define output sections */
SECTIONS
{

    . = CONFIG_XIP_BASE;

    STARTUP_SECTION

    .literal :
    {
        . = ALIGN(4);
        *(.literal)
        *(.literal*)
    }

    TEXT_SECTION(4)

    . = CONFIG_RAM_BASE;

/*
 * From this point, stuff is considered writable and will be copied to RAM
 */
	__data_loc = ALIGN(4);		/* location in file */
	. = PAGE_OFFSET;       /* location in memory */
#undef LOAD_OFFSET
#define LOAD_OFFSET (PAGE_OFFSET - __data_loc)

    RODATA_SECTION(4)

    RWDATA_SECTION(4, 4)

    INIT_TEXT_SECTION(4)

    INIT_DATA_SECTION(4)

    PROVIDE(_ld_kernel_size = . - _ld_text_start);

    BSS_SECTION(4, 0, 4)
}

