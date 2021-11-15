/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <mm.h>
#include <asm/pgtable.h>
#include <asm/entry.h>
#include <asm/percpu.h>
#include <asm/regs.h>
#include <asm/page.h>
#include <asm/gdt.h>

struct doublefault_stack {
    struct tss_entry tss;
    char stack[PAGE_SIZE - sizeof(struct tss_entry)];
} __aligned(PAGE_SIZE);

DEFINE_PERCPU(struct doublefault_stack, doublefault_stack) = {
    .tss = {
        .eip = (uint32_t)entry_double_fault,
        .cs = GDT_ENTRY_KERNEL_CS_BASE,
        .ds = GDT_ENTRY_KERNEL_DS_BASE,
        .es = GDT_ENTRY_KERNEL_DS_BASE,
        .ss = GDT_ENTRY_KERNEL_DS_BASE,
        .eflags = EFLAGS_FIXED,
        // .cr3 = va_to_pa(page_dir),
    },
};

void doublefault_init(void)
{
    struct doublefault_stack *dfstack = thiscpu_ptr(&doublefault_stack);

    dfstack->tss.esp = (uint32_t)dfstack->stack + sizeof(dfstack->stack);
    gdte_sys_set(GDT_ENTRY_DF_TSS, GDT_TYPE_TSSA,
                (uint32_t)&dfstack->tss, sizeof(dfstack->tss));
}
