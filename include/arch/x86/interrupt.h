/*
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  include/arch/x86/interrupt.h
 *  x86 interrupt define
 *  (C) 2020 wzl finishing
 * 
 *  Change Logs:
 *  Date            Notes
 *  2021-02-26      first version 
 * 
 */

#ifndef _ARCH_X86_INTERRUPT_H_
#define _ARCH_X86_INTERRUPT_H_

#ifndef __ASSEMBLY__
struct idt_bits {
    union {
        struct {
            uint16_t zero0:8;   // Must write 0
            uint16_t type:3;    // Decriptor type (task / interrut / trap)
            uint16_t d:1;       // Defaults type (0:16bit 1:32bit)
            uint16_t zero1:1;   // Must write 0
            uint16_t dpl:2;     // Decriptor privilege level (ring 0-3)
            uint16_t p:1;       // Interrupt present (0:non-existent 1:In memory)
        };
        uint16_t val;
    };
} __packed;

struct idt_entry{
    uint16_t offsetl;       // Offsetl to procedure entry point
    uint16_t segment;       // Segment Selector for destination code segment
    struct idt_bits bits;   // 
    uint16_t offseth;       // Offseth to procedure entry point
} __packed;

struct idt_table{
    uint16_t limit;             // IDT_ENTRY limit (byte)
    struct idt_entry *idt;      // Point to IDT_ENTRY
} __packed;

enum idt_type {
    IDT_TYPE_TASK       = 0x5,  // Task Gate
    IDT_TYPE_INTERRUPT  = 0x6,  // Interrupt Gate
    IDT_TYPE_TRAP       = 0x7,  // Trap Gate
};

enum idt_dpl {
    IDT_DPL_RING0       = 0x00,
    IDT_DPL_RING1       = 0x01,
    IDT_DPL_RING2       = 0x02,
    IDT_DPL_RING3       = 0x03,
};

enum idt_d {
    IDT_D_16    = 0x00,
    IDT_D_32    = 0x01,
};

enum fault_vector {
/*  Mnemonic        Vector     Description                      Source                                                                      */
    DE_FAULT        = 0,    /* Divide Error                     DIV and IDIV instructions.                                                  */
    DB_FAULT        = 1,    /* Debug Exception                  Instruction, data, and I/O breakpoints; single-step; and others.            */
    NMI_INTERRUPT   = 2,    /* NMI Interrupt                    Non-Maskable Interrupt                                                      */
    BP_TRAP         = 3,    /* Breakpoint                       Breakpoint Exception                                                        */
    OF_TRAP         = 4,    /* Overflow                         Overflow                                                                    */
    BR_FAULT        = 5,    /* BOUND Range Exceeded             BOUND instruction                                                           */
    UD_FAULT        = 6,    /* Invalid Opcode                   UD instruction or reserved opcode                                           */
    NM_FAULT        = 7,    /* Device Not Available             Floating-point or WAIT/FWAIT instruction.                                   */
    DF_ABORT        = 8,    /* Double Fault                     Any instruction that can generate an exception, an NMI, or an INTR.         */
    TS_FAULT        = 10,   /* Coprocessor Segment Overrun      Task switch or TSS access.                                                  */
    NP_FAULT        = 11,   /* Segment Not Present              Loading segment registers or accessing system segments.                     */
    SS_FAULT        = 12,   /* Stack-Segment Fault              Stack operations and SS register loads.                                     */
    GP_FAULT        = 13,   /* General Protection               Any memory reference and other protection checks.                           */
    PF_FAULT        = 14,   /* Page Fault                       Any memory reference.                                                       */
    MF_FAULT        = 16,   /* x87 FPU Floating-Point Error     x87 FPU floating-point or WAIT/FWAIT                                        */
    AC_FAULT        = 17,   /* Alignment Check                  Any data reference in memory                                                */
    MC_ABORT        = 18,   /* Machine Check                    Error codes (if any) and source are model dependent                         */
    XM_FAULT        = 19,   /* SIMD Floating-Point Exception    SSE/SSE2/SSE3 floating-point instructions                                   */
    VE_FAULT        = 20,   /* Virtualization Exception         EPT violations                                                              */
    CP_FAULT        = 21,   /* Control Protection Exception     RET, IRET, RSTORSSP, and SETSSBSY                                           */
};

#endif  /* __ASSEMBLY__ */

#define IRQ_EXTERNAL    0x40
#define IRQ_NR_MAX      256

#endif
