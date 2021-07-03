OUTPUT_FORMAT("elf32-littlearm")
OUTPUT_ARCH(arm)


/* Entry Point */
ENTRY(_start)

/* Define output sections */
SECTIONS
{
    . = 0x00000000;
    
    _ld_entry = .;
    
    .setup :
    {
        . = ALIGN(4);
        *(.setup)           /* .text sections (code) */
        *(.setup*)          /* .text* sections (code) */
    }
    
    .text :
    {
        . = ALIGN(4);
        *(.text)           /* .text sections (code) */
        *(.text*)          /* .text* sections (code) */
    }

    .rodata :
    {
        . = ALIGN(4);
        *(.rodata)           /* .data sections */
        *(.rodata*)          /* .data* sections */
    }
    
    .data :
    {
        . = ALIGN(4);
        *(.data)           /* .data sections */
        *(.data*)          /* .data* sections */
    }

    PROVIDE(_ld_spl_size = . - _ld_entry);
    
    .kernel :
    {
        . = ALIGN(4);
        _ld_skernel = .;
        *(.kernel)
        *(.kernel*)
        . = ALIGN(4);
        _ld_ekernel = .;
    }
    
    /* Uninitialized data section */
    .bss (NOLOAD) :
    {
        . = ALIGN(4);
        _ld_sbss = .;
        *(.bss)
        *(.bss*)
        . = ALIGN(4);
        _ld_ebss = .;
    }    

}

