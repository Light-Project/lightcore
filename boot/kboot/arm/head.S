#include <linkage.h>
#include <size.h>

#include "efi-header.S"

    .section .setup,"ax"
    
GLOBAL(_start)

    .rept   7
    MZ_HEADER
    .endr
    
    b       init
    
    EFI_HEADER
    
init:   
    /* save boot para */ 
    ldr     lr, =boot_para
    stmea   lr, {r0-r13}
    
Bss_segment_Init:
    movs    r0, #0
    ldr     r1, =_ld_bss_start
    ldr     r2, =_ld_bss_end
    b       Bss_loop
1:
    str     r0, [r1], #4
Bss_loop:
    cmp     r1, r2
    bcc     1b

env:
    ldr     sp, =_ld_boot_stack
    
    bl      main
    b       halt
    
END(_start)

ENTRY(boot_para)
.rept 14
    .long 0
.endr
END(boot_para)

/* jmp to kernel */    
GLOBAL(kernel_start)
    /* recovery boot para */
    ldr     lr, =(boot_para + 56)
    ldmed   lr, {r0-r13}
    
    ldr     pc, =(CONFIG_PAGE_OFFSET)
    
END(kernel_start)

/* Stop the CPU */
ENTRY(halt)
    nop
    b   halt
END(halt)
