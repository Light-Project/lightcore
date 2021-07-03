#include <asm-generic/kernel.lds.h>
#include <romcall.lds.h>
#include <size.h>

#define stack_size (size_1KiB * 8)

OUTPUT_ARCH(xtensa)

/* Entry Point */
ENTRY(_start)

MEMORY
{
    FLASH (rx)  : ORIGIN = 0x400C3000, LENGTH = CONFIG_XIP_BASE - 0x400C2000
    IRAM  (rwx) : ORIGIN = 0x40080000, LENGTH = 0x20000
    ICACHE(rwx) : ORIGIN = 0x40078000, LENGTH = 0x8000
    DRAM  (rw)  : ORIGIN = 0x3fff0000, LENGTH = 0x10000
}
/* Define output sections */
SECTIONS
{
    .text :
    {    
        KEEP(*(.startup));
        *(.literal)
        *(.literal*)
        *(.text)
        *(.text*)
        *(.ksymtab)
    } > IRAM AT > FLASH

    .vector :
    {
        . = 0x0;
        KEEP(*(.Window.Vector));
        . = 0x180;
        KEEP(*(.Level2Int.Vector));
        . = 0x1c0;
        KEEP(*(.Level3Int.Vector));
        . = 0x200;
        KEEP(*(.Level4Int.Vector));
        . = 0x240;
        KEEP(*(.Level5Int.Vector));
        . = 0x280;
        KEEP(*(.Debug.Vector));
        . = 0x2c0;
        KEEP(*(.NMI.Vector));
        . = 0x300;
        KEEP(*(.Kernel.Vector));
        . = 0x340;
        KEEP(*(.User.Vector));
        . = 0x3C0;
        KEEP(*(.Double.Vector));
        . = 0x400;
        *(.*Vector.literal)
    } > ICACHE AT > FLASH
    
    .data :
    {
        . = ALIGN(4);
        *(.rodata)
        *(.rodata*)
        *(.data)
        *(.data*)
    } > DRAM AT > FLASH

    .bss (NOLOAD) :
    {
        . = ALIGN(4);
        PROVIDE(_ld_sbss = .);
        *(.bss)
        *(.bss*)
        . = ALIGN(4);
        PROVIDE(_ld_ebss = .);
    } > DRAM
    
    .stack(NOLOAD) :
    {
        . = ALIGN(4);
        _ld_stack_start = .;
        . = _ld_stack_start + stack_size;
        . = ALIGN(4);
        _ld_stack_end = .;
    } > DRAM
    
    DWARF_DEBUG  
    
    /* Sections to be discarded */
    DISCARDS
}
