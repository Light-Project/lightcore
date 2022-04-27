/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <kernel.h>
#include <irqflags.h>
#include <asm/idt.h>

#define IDT_BITS(Ist, Type, Dpl) \
    .bits.ist = (Ist), .bits.type = (Type), .bits.dpl = (Dpl), \
    .bits.d = IDT_D_32, .bits.p = 1, .bits.zero0 = 0, .bits.zero1 = 0

/* Interrupt gate */
#define IDT_INTERRUPT(vec, fn) \
    .vector = (vec), .handle = (fn), .segment = GDT_ENTRY_KERNEL_CS_BASE, \
    IDT_BITS(0, IDT_TYPE_INTERRUPT, IDT_DPL_RING0)

/* System gate */
#define IDT_SYSTEM(vec, fn) \
    .vector = (vec), .handle = (fn), .segment = GDT_ENTRY_KERNEL_CS_BASE, \
    IDT_BITS(0, IDT_TYPE_INTERRUPT, IDT_DPL_RING3)

/* Task gate */
#define IDT_TASK(vec, seg) \
    .vector = (vec), .handle = NULL, .segment = (seg << 3), \
    IDT_BITS(0, IDT_TYPE_TASK, IDT_DPL_RING0)

/* Interrupt gate own stack */
#ifdef CONFIG_ARCH_X86_64
# define IDT_STACK(vec, fn, ist) \
    .vector = (vec), .handle = (fn), .segment = GDT_ENTRY_KERNEL_CS_BASE, \
    IDT_BITS(ist + 1, IDT_TYPE_INTERRUPT, IDT_DPL_RING3)
#else
# define IDT_STACK(vec, fn, ist) IDT_INTERRUPT(vec, fn)
#endif

static struct idt_entry idt_entry[IRQ_NR_MAX];
static struct idt_table idt_struct = {
    .idt = idt_entry,
    .limit = sizeof(idt_entry) - 1,
};

struct idt_data {
    irqnr_t vector;
    void *handle;
    uint16_t segment;
    struct idt_bits bits;
};

static const __initconst struct idt_data idt_data[] = {
    /* Interrupt gate */
    { IDT_INTERRUPT(TRAP_DE, entry_divide_error) },
    { IDT_INTERRUPT(TRAP_BP, entry_breakpoint) },
    { IDT_INTERRUPT(TRAP_BR, entry_bounds) },
    { IDT_INTERRUPT(TRAP_UD, entry_invalid_opcode) },
    { IDT_INTERRUPT(TRAP_NM, entry_device_not_available) },
    { IDT_INTERRUPT(TRAP_TS, entry_invalid_tss) },
    { IDT_INTERRUPT(TRAP_NP, entry_segment_not_present) },
    { IDT_INTERRUPT(TRAP_SS, entry_stack_segment) },
    { IDT_INTERRUPT(TRAP_GP, entry_general_protection) },
    { IDT_INTERRUPT(TRAP_PF, entry_page_fault) },
    { IDT_INTERRUPT(TRAP_SP, entry_spurious_interrupt) },
    { IDT_INTERRUPT(TRAP_MF, entry_coprocessor_error) },
    { IDT_INTERRUPT(TRAP_AC, entry_alignment_check) },
    { IDT_INTERRUPT(TRAP_XM, entry_simd_error) },

    /* System interrupt gate */
    { IDT_SYSTEM(TRAP_OF, entry_overflow) },

    { IDT_STACK(TRAP_DB, entry_debug, ) },
    { IDT_STACK(TRAP_NMI, entry_nmi_interrupt, ) },

#ifdef CONFIG_ARCH_X86_32
    { IDT_SYSTEM(TRAP_SYSCALL, entry_syscall_80) },
    { IDT_TASK(TRAP_DF, GDT_ENTRY_DF_TSS) },
#endif

};

void arch_idt_int_gate(irqnr_t nr, void *handle)
{
    idt_entry[nr] = (struct idt_entry) {
        .offsetl  = (size_t)handle,
        .offseth  = (size_t)handle >> 16,
        .segment  = GDT_ENTRY_KERNEL_CS_BASE,
        IDT_BITS(0, IDT_TYPE_INTERRUPT, IDT_DPL_RING0),
    };
}

void __init arch_idt_setup(void)
{
    const struct idt_data *data;
    unsigned int count;

    for (count = 0; count < ARRAY_SIZE(idt_data); ++count) {
        data = &idt_data[count];
        idt_entry[data->vector].offsetl  = (size_t)data->handle;
        idt_entry[data->vector].offseth  = (size_t)data->handle >> 16;
        idt_entry[data->vector].segment  = data->segment;
        idt_entry[data->vector].bits.val = data->bits.val;
    }

    for (count = IRQ_EXTERNAL; count < IRQ_NR_MAX; ++count) {
        if (count == TRAP_SYSCALL)
            continue;
        arch_idt_int_gate(
            count,
            (void *)((size_t)entry_irq_vector +
            (0x10 * (count - IRQ_EXTERNAL)))
        );
    }

    idt_set(&idt_struct);
}
