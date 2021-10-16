/* SPDX-License-Identifier: GPL-2.0-or-later */
OUTPUT_ARCH(aarch64)
ENTRY(_start)

MEMORY
{
    RAM (xrw) : ORIGIN = 0xff8c2000, LENGTH = 192K
}

SECTIONS
{
    .setup : {
        . = ALIGN(4);
        *(.setup)
    } > RAM
    
    .text : {
        . = ALIGN(4);
        *(.text)
        *(.text*)
    } > RAM

    .rodata : {
        . = ALIGN(4);
        *(.rodata)
        *(.rodata*)
    } > RAM
    
    .data : {
        . = ALIGN(4);
        *(.data)
        *(.data*)
    } > RAM
    
    .bss (NOLOAD) : {
        . = ALIGN(4);
        _ld_sbss = .;
        *(.bss)
        *(.bss*)
        . = ALIGN(4);
        _ld_ebss = .;
    } > RAM  
}

