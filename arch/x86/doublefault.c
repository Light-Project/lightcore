/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <memory.h>
#include <asm/pgtable.h>
#include <asm/entry.h>
#include <asm/percpu.h>
#include <asm/regs.h>
#include <asm/page.h>
#include <asm/segment.h>

struct doublefault_stack {
    struct tss_entry tss;
    char stack[PAGE_SIZE - sizeof(struct tss_entry)];
} __packed __aligned(PAGE_SIZE);

DEFINE_PERCPU(struct doublefault_stack, doublefault_stacks) = {
    .tss = {
        .eip    = (size_t)entry_double_fault,
        .eflags = EFLAGS_FIXED,
        .cs     = GDT_ENTRY_KERNEL_CS_BASE,
        .ds     = GDT_ENTRY_KERNEL_DS_BASE,
        .es     = GDT_ENTRY_KERNEL_DS_BASE,
        .ss     = GDT_ENTRY_KERNEL_DS_BASE,
        .cr3    = va_to_pa(page_dir),
    },
};

void doublefault_init(void)
{
    struct doublefault_stack *dfstack;

    dfstack = thiscpu_ptr(&doublefault_stacks);
    dfstack->tss.esp = (size_t)dfstack->stack + sizeof(dfstack->stack);

    gdte_sys_set(
        GDT_ENTRY_DF_TSS, GDT_TYPE_TSSA,
        (size_t)&dfstack->tss,
        sizeof(dfstack->tss)
    );
}
