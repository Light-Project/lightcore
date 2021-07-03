    .arm
    .section	.text

.global _start
_start:
    /* Jump to the save_fel */
    ldr pc, =Vectors_Table
    /* Boot head information for BROM */
    .byte 'e', 'G', 'O', 'N', '.', 'B', 'T', '0'
    .long 0, _ld_spl_size
    .byte 'S', 'P', 'L', 2
    .long 0, 0
    .long 0, 0, 0, 0    // 0x20 - dram size 
    .long 0, 0, 0, 0    // 0x28 - boot type
    .long 0, 0, 0, 0    // 0x40 - boot params 
    .long 0, 0, 0, 0

Vectors_Table:
    ldr pc, =save_fel
    ldr pc, =Undefined0_Handle
    ldr pc, =SVC_Handle
    ldr pc, =Perfetch_Abort_Handle
    ldr pc, =Data_Abort_Handle
    ldr pc, =Undefined1_Handle
    ldr pc, =IRQ_Handle
    ldr pc, =FIQ_Handle

.weak Undefined0_Handle
Undefined0_Handle:
    b   halt
    
.weak SVC_Handle
SVC_Handle:
    b   halt

.weak Perfetch_Abort_Handle
Perfetch_Abort_Handle:
    b   halt

.weak Data_Abort_Handle
Data_Abort_Handle:
    b   halt

.weak Undefined1_Handle
Undefined1_Handle:
    b   halt

.weak IRQ_Handle
IRQ_Handle:
    b   halt

.weak FIQ_Handle
FIQ_Handle:
    b   halt

.global real_start
real_start:
    
    /* Set Stack pointer */
    ldr   SP,=0x10000

    /* 跳转到main */
    bl  main
    b   rec_fel

.global save_fel
save_fel:

    /* Save boot params to 0x00000040 */
    ldr r0, =0x00000040
    str sp, [r0, #0]
    str lr, [r0, #4]
    mrs lr, cpsr
    str lr, [r0, #8]
    mrc p15, 0, lr, c1, c0, 0
    str lr, [r0, #12]
    mrc p15, 0, lr, c12, c0, 0
    str lr, [r0, #16]
    mrc p15, 0, lr, c1, c0, 0
    str lr, [r0, #20]
    b   real_start

.global rec_fel
rec_fel:
    ldr r0, =0x00000040
    ldr sp, [r0, #0]
    ldr lr, [r0, #4]
    ldr r1, [r0, #20]
    mcr p15, 0, r1, c1, c0, 0
    ldr r1, [r0, #16]
    mcr p15, 0, r1, c12, c0, 0
    ldr r1, [r0, #12]
    mcr p15, 0, r1, c1, c0, 0
    ldr r1, [r0, #8]
    msr cpsr, r1
    bx lr
    
halt:
    b   .
        
