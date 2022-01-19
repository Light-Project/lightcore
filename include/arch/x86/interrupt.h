/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _ARCH_X86_INTERRUPT_H_
#define _ARCH_X86_INTERRUPT_H_

#include <types.h>

#ifndef __ASSEMBLY__

struct idt_bits {
    union {
        struct {
            uint16_t ist:3;
            uint16_t zero0:5;   /* Must write 0 */
            uint16_t type:3;    /* Decriptor type (task / interrut / trap) */
            uint16_t d:1;       /* Defaults type (0:16bit 1:32bit) */
            uint16_t zero1:1;   /* Must write 0 */
            uint16_t dpl:2;     /* Decriptor privilege level (ring 0-3) */
            uint16_t p:1;       /* Interrupt present (0:non-existent 1:In memory) */
        };
        uint16_t val;
    };
} __packed;

struct idt_entry {
    uint16_t offsetl;           /* Offsetl to procedure entry point */
    uint16_t segment;           /* Segment Selector for destination code segment */
    struct idt_bits bits;       /*  */
    uint16_t offseth;           /* Offseth to procedure entry point */
} __packed;

struct idt_table {
    uint16_t limit;             /* IDT_ENTRY limit (byte) */
    struct idt_entry *idt;      /* Point to IDT_ENTRY */
} __packed;

#define IDT_TYPE_TASK           0x05    /* Task Gate */
#define IDT_TYPE_INTERRUPT      0x06    /* Interrupt Gate */
#define IDT_TYPE_TRAP           0x07    /* Trap Gate */

#define IDT_DPL_RING0           0x00
#define IDT_DPL_RING1           0x01
#define IDT_DPL_RING2           0x02
#define IDT_DPL_RING3           0x03

#define IDT_D_16                0x00
#define IDT_D_32                0x01

enum trap_vector {
/*  Mnemonic        Vector     Description                      Source                                                                      */
    TRAP_DE         = 0,    /* Divide Error                     DIV and IDIV instructions.                                                  */
    TRAP_DB         = 1,    /* Debug Exception                  Instruction, data, and I/O breakpoints; single-step; and others.            */
    TRAP_NMI        = 2,    /* NMI Interrupt                    Non-Maskable Interrupt                                                      */
    TRAP_BP         = 3,    /* Breakpoint                       Breakpoint Exception                                                        */
    TRAP_OF         = 4,    /* Overflow                         Overflow                                                                    */
    TRAP_BR         = 5,    /* BOUND Range Exceeded             BOUND instruction                                                           */
    TRAP_UD         = 6,    /* Invalid Opcode                   UD instruction or reserved opcode                                           */
    TRAP_NM         = 7,    /* Device Not Available             Floating-point or WAIT/FWAIT instruction.                                   */
    TRAP_DF         = 8,    /* Double Fault                     Any instruction that can generate an exception, an NMI, or an INTR.         */
    TRAP_TS         = 10,   /* Coprocessor Segment Overrun      Task switch or TSS access.                                                  */
    TRAP_NP         = 11,   /* Segment Not Present              Loading segment registers or accessing system segments.                     */
    TRAP_SS         = 12,   /* Stack-Segment Fault              Stack operations and SS register loads.                                     */
    TRAP_GP         = 13,   /* General Protection               Any memory reference and other protection checks.                           */
    TRAP_PF         = 14,   /* Page Fault                       Any memory reference.                                                       */
    TRAP_SP         = 15,   /* Spurious Interrupt                                                                                           */
    TRAP_MF         = 16,   /* x87 FPU Floating-Point Error     x87 FPU floating-point or WAIT/FWAIT                                        */
    TRAP_AC         = 17,   /* Alignment Check                  Any data reference in memory                                                */
    TRAP_MC         = 18,   /* Machine Check                    Error codes (if any) and source are model dependent                         */
    TRAP_XM         = 19,   /* SIMD Floating-Point Exception    SSE/SSE2/SSE3 floating-point instructions                                   */
    TRAP_VE         = 20,   /* Virtualization Exception         EPT violations                                                              */
    TRAP_CP         = 21,   /* Control Protection Exception     RET, IRET, RSTORSSP, and SETSSBSY                                           */
};

#endif  /* __ASSEMBLY__ */
#endif  /* _ARCH_X86_INTERRUPT_H_ */
