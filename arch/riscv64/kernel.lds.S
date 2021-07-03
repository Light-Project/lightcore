#include <generated/autoconf.h>

OUTPUT_ARCH(arm)
ENTRY(startup_head)

SECTIONS
{
    . = CONFIG_PAGE_OFFSET;
    
    .startup :
    {
        . = ALIGN(4);
        PROVIDE(_ld_sstartup = .);
        *(.startup)           /* .startup sections (startup) */
        *(.startup*)          /* .startup* sections (startup) */
        . = ALIGN(4);
        PROVIDE(_ld_estartup = .);
    } 
    
    .text :
    {
        . = ALIGN(4);
        PROVIDE(_ld_stext = .);
        *(.text)           /* .text sections (code) */
        *(.text*)          /* .text* sections (code) */
        . = ALIGN(4);
        PROVIDE(_ld_etext = .);
    }
        
    .initcall :
    {
        . = ALIGN(4);
        PROVIDE(_ld_sinitcall = .);
        KEEP(*(.initcall_0))
        KEEP(*(.initcall_1))
        KEEP(*(.initcall_2))
        KEEP(*(.initcall_3))
        KEEP(*(.initcall_4))
        KEEP(*(.initcall_5))
        KEEP(*(.initcall_6))
        KEEP(*(.initcall_7))
        KEEP(*(.initcall_8))
        KEEP(*(.initcall_9))
        . = ALIGN(4);
        PROVIDE(_ld_einitcall = .);
    }
    
    .exitcall :
    {
        . = ALIGN(4);
        PROVIDE(_ld_sexitcall = .);
        KEEP(*(.exitcall_9))
        KEEP(*(.exitcall_8))
        KEEP(*(.exitcall_7))
        KEEP(*(.exitcall_6))
        KEEP(*(.exitcall_5))
        KEEP(*(.exitcall_4))
        KEEP(*(.exitcall_3))
        KEEP(*(.exitcall_2))
        KEEP(*(.exitcall_1))
        KEEP(*(.exitcall_0))
        . = ALIGN(4);
        PROVIDE(_ld_eexitcall = .);
    }
    
    .ksymtab :
    {
        . = ALIGN(4);
        PROVIDE(_ld_sksymtab = .);
        KEEP(*(.ksymtab))
        KEEP(*(.ksymtab*))
        . = ALIGN(4);
        PROVIDE(_ld_eksymtab = .);
    }    
    
    .dts :
    {
        . = ALIGN(4);
        PROVIDE(_ld_sdts = .);
        KEEP(*(.dts))
        KEEP(*(.dts*))
        . = ALIGN(4);
        PROVIDE(_ld_edts = .);
    }    
    
    .rodata :
    {
        . = ALIGN(4);
        PROVIDE(_ld_sidata = .);
        *(.rodata)         /* .rodata sections (code) */
        *(.rodata*)        /* .rodata* sections (code) */
        . = ALIGN(4);
        PROVIDE(_ld_eidata = .);
    }
    
    .data :
    {
        . = ALIGN(4);
        PROVIDE(_ld_sdata = .);
        *(.data)           /* .data sections */
        *(.data*)          /* .data* sections */
        . = ALIGN(4);
        PROVIDE(_ld_edata = .);
    }

    /* Uninitialized data section */
    .bss :
    {
        . = ALIGN(4);
        PROVIDE(_ld_bss_start = .);
        *(.bss)
        *(.bss*)
        . = ALIGN(4);
        PROVIDE(_ld_bss_end = .);
    }    
    
    /* Uninitialized data section */
    .heap :
    {
        . = ALIGN(4);
        PROVIDE(_ld_sheap = .);
        *(.heap)
        *(.heap*)
        . = ALIGN(4);
        PROVIDE(_ld_eheap = .);
    }
    
    PROVIDE(_ld_kernel_size = _ld_edata - _ld_sstartup);
}

