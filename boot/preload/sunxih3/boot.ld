OUTPUT_FORMAT("elf32-littlearm", "elf32-bigarm", "elf32-littlearm")
OUTPUT_ARCH(arm)


/* Entry Point */
ENTRY(_start)

MEMORY
{
    RAM (xrw)       : ORIGIN = 0x00000000, LENGTH = 64M
}

/* Define output sections */
SECTIONS
{
    .text :
    {
        . = ALIGN(4);
        _ld_spl_start = .;
        *(.text)           /* .text sections (code) */
        *(.text*)          /* .text* sections (code) */
        . = ALIGN(4);
        _ld_spl_end = .;
    } >RAM
    _ld_spl_size = _ld_spl_end - _ld_spl_start;
    
    .rodata :
    {
        . = ALIGN(4);
        *(.rodata)          /* .rodata sections */
        *(.rodata*)         /* .rodata* sections */
        . = ALIGN(4);
    } >RAM
    
    .data :
    {
        . = ALIGN(4);
        *(.data)           /* .data sections */
        *(.data*)          /* .data* sections */
        . = ALIGN(4);
    } >RAM

    /* Uninitialized data section */
    .bss :
    {
        . = ALIGN(4);
        *(.bss)
        *(.bss*)
        *(COMMON)
        . = ALIGN(4);
    } >RAM
}

