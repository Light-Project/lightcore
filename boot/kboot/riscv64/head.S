#include <linkage.h>
#include <size.h>
#include <abi/entry.h>

    .section .setup,"ax"

GLOBAL(_start)

    /* save boot para */
    lrw     r1, boot_para
    mov     sp, r1
    stm     r2-r15, (sp)
    
    /* Turn off dcache */
    mfcr    r0, ccr
    bclri   r0, 3   /* WB */
    mtcr    r0, ccr
    
    SETUP_MMU
    
Bss_segment_Init:
    lrw     r1, 0
    lrw     r8, _ld_bss_start
    lrw     r9, _ld_bss_end
    jmpi    Bss_loop
1:
    st.w    r1, (r8, 0)
    addi    r8, r8, 4
Bss_loop:
    cmphs   r8, r9
    bf      1b

env:
    lrw     r1, (boot_stack + (size_1KB * 4))
    mov     sp, r1
    
    jsri    main
END(_start)

/* jmp to kernel */    
GLOBAL(kernel_start)
    /* recovery boot para */
    lrw     r1, (boot_para + 56)
    mov     sp, r1
    ldm     r2-r15, (sp)
    
    lrw     r1, CONFIG_PAGE_OFFSET + CONFIG_RAM_BASE
    jmp     r1
    
END(kernel_start)

/* Stop the CPU and most peripherals */    
GLOBAL(halt)
    stop
    br      halt
END(halt)

    .section .bss
boot_para:
    .long 0,0,0,0   /* r2, r3, r4, r5 */
    .long 0,0,0,0   /* r6, r7, r8, r9 */
    .long 0,0,0,0   /* r10, r11, r12, r13 */
    .long 0,0       /* r14, r15 */
    .section    .text
    
.global _start
_start:
Bss_segment_Init:
    la      a0, _ld_sbss
    la      a1, _ld_ebss
    jal     Bss_loop
1:
    sd      zero, (a0)
    addi    a0, a0, 8
Bss_loop:
    bltu    a0, a1, 1b
init:
	lui     sp, 0x80001
    jal     main
    
halt:
    jal    .
 
