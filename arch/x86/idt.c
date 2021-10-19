/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <types.h>
#include <kernel.h>
#include <state.h>
#include <mm/vmem.h>

#include <asm/idt.h>
#include <asm/entry.h>

#define IDT_BITS(Ist, Type, Dpl) \
    .bits.ist = (Ist), .bits.type = (Type), .bits.dpl = (Dpl), \
    .bits.d = IDT_D_32, .bits.p = 1, .bits.zero0 = 0, .bits.zero1 = 0

/* Interrupt gate */
#define IDT_INTERRUPT(vec, fn) \
    .vector = (vec), .handle = (fn), .segment = GDT_ENTRY_KERNEL_CS_BASE, \
    IDT_BITS(0, IDT_TYPE_INTERRUPT, IDT_DPL_RING0)

/* Task gate */
#define IDT_TASK(vec, seg) \
    .vector = (vec), .handle = NULL, .segment = seg, \
    IDT_BITS(0, IDT_TYPE_TASK, IDT_DPL_RING0)

/* System interrupt gate */
#define IDT_SYSTEM(vec, fn) \
    .vector = (vec), .handle = (fn), .segment = GDT_ENTRY_KERNEL_CS_BASE, \
    IDT_BITS(0, IDT_TYPE_INTERRUPT, IDT_DPL_RING3)

/* Interrupt gate with own stack */
#ifdef CONFIG_ARCH_X86_64
#define IDT_STACK(vec, fn, ist) \
    .vector = (vec), .handle = (fn), .segment = GDT_ENTRY_KERNEL_CS_BASE, \
    IDT_BITS(ist + 1, IDT_TYPE_INTERRUPT, IDT_DPL_RING3)
#else
#define IDT_STACK(vec, fn, ist) IDT_INTERRUPT(vec, fn)
#endif

static struct idt_entry idt_entry[IRQ_NR_MAX];
static struct idt_table idt_struct = {
    .idt = idt_entry,
    .limit = sizeof(idt_entry) - 1,
};

static const __initconst struct idt_data idt_data[] = {
    /* Interrupt gate */
    {IDT_INTERRUPT(DE_FAULT, entry_divide_error)},
    {IDT_INTERRUPT(BP_TRAP,  entry_breakpoint)},
    {IDT_INTERRUPT(BR_FAULT, entry_bounds)},
    {IDT_INTERRUPT(UD_FAULT, entry_invalid_opcode)},
    {IDT_INTERRUPT(NM_FAULT, entry_device_not_available)},
    {IDT_INTERRUPT(TS_FAULT, entry_invalid_tss)},
    {IDT_INTERRUPT(NP_FAULT, entry_segment_not_present)},
    {IDT_INTERRUPT(SS_FAULT, entry_stack_segment)},
    {IDT_INTERRUPT(GP_FAULT, entry_general_protection)},
    {IDT_INTERRUPT(PF_FAULT, entry_general_protection)},
    {IDT_INTERRUPT(SP_TRAP,  entry_spurious_interrupt)},
    {IDT_INTERRUPT(MF_FAULT, entry_coprocessor_error)},
    {IDT_INTERRUPT(AC_FAULT, entry_alignment_check)},
    {IDT_INTERRUPT(XM_FAULT, entry_simd_error)},

    /* System interrupt gate */
    {IDT_SYSTEM(OF_TRAP, entry_overflow)},
    {IDT_SYSTEM(SYSCALL_TRAP, entry_syscall_80)},

    {IDT_TASK(DF_ABORT, GDT_ENTRY_DF_TSS_BASE)},
    {IDT_STACK(DB_FAULT, entry_debug, )},
    {IDT_STACK(NMI_INT,  entry_nmi_interrupt, )},
};

void idt_int_gate(irqnr_t nr, void *handle)
{
    idt_entry[nr] = (struct idt_entry) {
        .offsetl  = (size_t)handle,
        .offseth  = (size_t)handle >> 16,
        .segment  = GDT_ENTRY_KERNEL_CS_BASE,
        IDT_BITS(0, IDT_TYPE_INTERRUPT, IDT_DPL_RING0),
    };
}

void __init idt_setup(void)
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

    for (count = IRQ_EXTERNAL; count < IRQ_NR_MAX; ++count)
        idt_int_gate(count, (void *)((size_t)entry_generic_interrupt +
        (0x20 * (count - IRQ_EXTERNAL))));

    idt_set(&idt_struct);
}
