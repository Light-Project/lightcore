#include <asm-generic/kernel.lds.h>
#include <size.h>

#define stack_size (size_1KiB * 8)

ENTRY(_start)

/* Declare storage space */
MEMORY
{
  INT       (rx)    : ORIGIN = 0x000,   LENGTH = 0x0400
  BIOSDATA  (rx)    : ORIGIN = 0x400,   LENGTH = 0x0100
  MBRSTACK  (rw)    : ORIGIN = 0x500,   LENGTH = 0x02c0
  MBR       (rx)    : ORIGIN = 0x7c00,  LENGTH = 0x01b8
  DPTSIG    (rw)    : ORIGIN = 0x7db8,  LENGTH = 0x0006 
  DPT       (rw)    : ORIGIN = 0x7dbe,  LENGTH = 0x0040
  SIG       (rw)    : ORIGIN = 0x7dfe,  LENGTH = 0x0002
  
  /* mem:  0x10000 - 0x7e00 = 0x8200 */
  /* disk:         512 * 62 = 0x7C00 */
  RAM       (rwx)   : ORIGIN = 0x7e00,  LENGTH = 0x7C00
}

SECTIONS
{
    .setup :
    {
        PROVIDE(_ld_setup_start = .);
        KEEP(*(.setup))
        PROVIDE(_ld_esetup = .);
    } > MBR
    
    .dptsign :
    {
        KEEP(*(.dptsign))
    } > DPTSIG
    
    .signature :
    {
        SHORT(0xaa55)
    } > SIG
    
    .text :
    {
        . = ALIGN(4);
        PROVIDE(_ld_text_start = .);
        *(.text) 
        *(.text*) 
        . = ALIGN(4);
        PROVIDE(_ld_text_end = .);
    } > RAM
    
    .rodata :
    {
        PROVIDE(_ld_rodata_start = .);
        *(.rodata)         /* .rodata sections (code) */
        *(.rodata*)        /* .rodata* sections (code) */
        PROVIDE(_ld_rodata_end = .);
    } > RAM
    
    .data :
    {
        PROVIDE(_ld_data_start = .);
        *(.data)           /* .data sections */
        *(.data*)          /* .data* sections */
        PROVIDE(_ld_data_end = .);
    } > RAM
    
    .dts :
    {
        . = ALIGN(4);
        PROVIDE(_ld_dts_start = .);
        KEEP(*(.dts))
        . = ALIGN(4);
    } > RAM
    
    .align :
    {
        PROVIDE(_ld_align_start = .);
        SHORT(0xaa55)
        . = ALIGN(512);
        PROVIDE(_ld_align_end = .);
    } > RAM
    
    /* Uninitialized data section */
    .bss (NOLOAD) :
    {
        PROVIDE(_ld_bss_start = .);
        *(.bss)
        *(.bss*)
        PROVIDE(_ld_bss_end = .);
    } > RAM
    
    .stack(NOLOAD) :
    {
        . = ALIGN(4);
        PROVIDE(_ld_stack_start = .);
        . = _ld_stack_start + stack_size;
        . = ALIGN(4);
        PROVIDE(_ld_stack_end = .);
    } > RAM
    
    PROVIDE(_ld_block_start = (_ld_text_start - _ld_setup_start)/512);
    PROVIDE(_ld_block_num = (_ld_align_end - _ld_text_start)/512);
}
