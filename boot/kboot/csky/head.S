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
    
    lrw     r1, 0x80305000
    ld.w    r2, (r1, 0)
    bseti   r2, 10
    st.w    r2, (r1, 0)
    ld.w    r2, (r1, 8)
    bseti   r2, 10
    st.w    r2, (r1, 8)
    
Bss_segment_Init:
    lrw     r1, 0
    lrw     r2, _ld_bss_start
    lrw     r3, _ld_bss_end
    jmpi    Bss_loop
1:
    st.w    r1, (r2, 0)
    addi    r2, 4
Bss_loop:
    cmphs   r2, r3
    bf      1b
    
getpc:
    jsri    env
    
env:
    lrw     r1, _ld_boot_stack
    mov     sp, r1
    
    mov     r2, r15
    lrw     r8, (getpc - 2)
    sub     r2, r8
    
    lrw     r3, _ld_size
    
    jsri    main
END(_start)

ENTRY(boot_para)
.rept 14
    .long 0
.endr
END(boot_para)
    
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
    jmpi    halt
END(halt)
