/*
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  bootloader/x86/lib/interrupt.c
 *  Operation of IDT
 *  (C) 2020 wzl
 * 
 *  Change Logs:
 *  Date            Notes
 *  2021-02-26      first version 
 * 
 */

#include <types.h>
#include <stddef.h>
#include <state.h>
#include <mm/vmem.h>

#include <asm/idt.h>
#include <asm/entry.h>

static struct idt_entry idt_entry[IRQ_NR_MAX];
static struct idt_table idt_struct = {
    .idt = idt_entry,
    .limit = sizeof(idt_entry) - 1,
};

static struct idt_data idt_data[IRQ_NR_MAX] = {
    {IDT_DATA_ENTRY(DE_FAULT, entry_divide_error)},
    {IDT_DATA_ENTRY(DB_FAULT, entry_general_protection)},
    {IDT_DATA_ENTRY(NMI_INTERRUPT, entry_general_protection)},
    {IDT_DATA_ENTRY(BP_TRAP, entry_general_protection)},
    {IDT_DATA_ENTRY(OF_TRAP, entry_general_protection)},
    {IDT_DATA_ENTRY(BR_FAULT, entry_general_protection)},
    {IDT_DATA_ENTRY(UD_FAULT, entry_general_protection)},
    {IDT_DATA_ENTRY(NM_FAULT, entry_general_protection)},
    {IDT_DATA_ENTRY(DF_ABORT, entry_general_protection)},
    {IDT_DATA_ENTRY(TS_FAULT, entry_general_protection)},
    {IDT_DATA_ENTRY(NP_FAULT, entry_general_protection)},
    {IDT_DATA_ENTRY(SS_FAULT, entry_general_protection)},
    {IDT_DATA_ENTRY(GP_FAULT, entry_general_protection)},
    {IDT_DATA_ENTRY(PF_FAULT, entry_general_protection)},
    {IDT_DATA_ENTRY(MF_FAULT, entry_general_protection)},
    {IDT_DATA_ENTRY(AC_FAULT, entry_general_protection)},
    {IDT_DATA_ENTRY(MC_ABORT, entry_general_protection)},
    {IDT_DATA_ENTRY(XM_FAULT, entry_general_protection)},
    {IDT_DATA_ENTRY(VE_FAULT, entry_general_protection)},
    {IDT_DATA_ENTRY(CP_FAULT, entry_general_protection)},
    {IDT_DATA_ENTRY(CP_FAULT, entry_general_protection)},
    {IDT_DATA_ENTRY(SYSCALL_TRAP, entry_syscall)},
};

void __init arch_idt_setup(void)
{
    unsigned int i;
    struct idt_data *idt;
    for(i = 0; i < IRQ_NR_MAX; ++i) {
        idt = &idt_data[i];
        if(idt->func)
            IDT_SET_KERNEL(idt_entry, idt->vector, idt->func);
        else
            IDT_SET_KERNEL(idt_entry, idt->vector, entry_undefine);
    }

    idt_load(&idt_struct);
}
