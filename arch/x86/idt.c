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

#define IDT_BITS_INT    0x8F00

#define IDT_BITS(_type, _dpl) \
    .bits.type = _type, .bits.dpl = _dpl, .bits.d = IDT_D_32, \
    .bits.p = 1, .bits.zero0 = 0, .bits.zero1 = 0,

#define IDT_INTERRUPT(vec, fn) \
    .vector = (vec), .handle = (fn), .segment = GDT_ENTRY_KERNEL_CS_BASE, \
    IDT_BITS(IDT_TYPE_INTERRUPT, IDT_DPL_RING0)

#define IDT_SYSTEM(vec, fn) \
    .vector = (vec), .handle = (fn), .segment = GDT_ENTRY_KERNEL_CS_BASE, \
    IDT_BITS(IDT_TYPE_INTERRUPT, IDT_DPL_RING3)

#define IDT_TASK(vec, seg) \
    .vector = (vec), .handle = 0, .segment = seg, \
    IDT_BITS(IDT_TYPE_TASK, IDT_DPL_RING3)

static struct idt_entry idt_entry[IRQ_NR_MAX];
static struct idt_table idt_struct = {
    .idt = idt_entry,
    .limit = sizeof(idt_entry) - 1,
};

static const __initconst struct idt_data idt_data[] = {
    {IDT_INTERRUPT(DE_FAULT, entry_divide_error)},
    {IDT_INTERRUPT(DB_FAULT, entry_general_protection)},
    {IDT_INTERRUPT(NMI_INTERRUPT, entry_general_protection)},
    {IDT_INTERRUPT(BP_TRAP, entry_general_protection)},
    {IDT_INTERRUPT(BR_FAULT, entry_general_protection)},
    {IDT_INTERRUPT(UD_FAULT, entry_general_protection)},
    {IDT_INTERRUPT(NM_FAULT, entry_general_protection)},
    {IDT_INTERRUPT(TS_FAULT, entry_general_protection)},
    {IDT_INTERRUPT(NP_FAULT, entry_general_protection)},
    {IDT_INTERRUPT(SS_FAULT, entry_general_protection)},
    {IDT_INTERRUPT(GP_FAULT, entry_general_protection)},
    {IDT_INTERRUPT(PF_FAULT, entry_general_protection)},
    {IDT_INTERRUPT(MF_FAULT, entry_general_protection)},
    {IDT_INTERRUPT(AC_FAULT, entry_general_protection)},
    {IDT_INTERRUPT(MC_ABORT, entry_general_protection)},
    {IDT_INTERRUPT(XM_FAULT, entry_general_protection)},
    {IDT_INTERRUPT(VE_FAULT, entry_general_protection)},
    {IDT_INTERRUPT(CP_FAULT, entry_general_protection)},

    {IDT_SYSTEM(OF_TRAP, entry_general_protection)},
    {IDT_SYSTEM(SYSCALL_TRAP, entry_syscall)},
    
    {IDT_TASK(DF_ABORT, GDT_ENTRY_DOUBLEFAULT_TSS)},
};

static void idt_int_gate(irqnr_t nr, void *handle)
{
    idt_entry[nr].offsetl  = (size_t)handle;
    idt_entry[nr].offseth  = (size_t)handle >> 16;
    idt_entry[nr].segment  = GDT_ENTRY_KERNEL_CS_BASE;
    idt_entry[nr].bits.val = IDT_BITS_INT;
}

static void idt_setup(void)
{
    const struct idt_data *data;
    unsigned int count;

    for(count = 0; count < ARRAY_SIZE(idt_data); ++count) {
        data = &idt_data[count];
        idt_entry[data->vector].offsetl = (size_t)data->handle;
        idt_entry[data->vector].offseth = (size_t)data->handle >> 16;
        idt_entry[data->vector].segment = data->segment;
        idt_entry[data->vector].bits    = data->bits;
    }

    for(count = IRQ_EXTERNAL; count < IRQ_NR_MAX; ++count)
        idt_int_gate(count, (void *)((size_t)entry_generic_interrupt + 
        (0x10 * (count - IRQ_EXTERNAL))));
}

void __init arch_idt_setup(void)
{

    idt_setup();
    idt_load(&idt_struct);
}
