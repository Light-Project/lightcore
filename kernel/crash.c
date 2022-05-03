/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <panic.h>
#include <memory.h>
#include <proc.h>
#include <irqflags.h>
#include <printk.h>
#include <export.h>

static LIST_HEAD(panic_works_list);
DEFINE_NOTIFIER_SPIN_HEAD(panic_notifier);

void __noreturn panic(const char *fmt, ...)
{
    char buf[256];
    va_list args;

    irq_local_disable();

    va_start(args, fmt);
    vsnprintf(buf, sizeof(buf), fmt, args);
    va_end(args);

    printk("Kernel Panic: %s\n", buf);
    printk("---[ end Kernel panic ]---\n");

    notifier_spin_chain_call(&panic_notifier, NULL, -1, NULL);
    proc_halt();
}
EXPORT_SYMBOL(panic);

struct crash_entry *crash_find(uintptr_t addr)
{
    struct crash_entry *walk;

    for (walk = (struct crash_entry *)_ld_data_bug_table_start;
         walk < (struct crash_entry *)_ld_data_bug_table_end; ++walk) {
        if (walk->addr == addr)
            return walk;
    }

    return NULL;
}

enum crash_type crash_report(uintptr_t addr, struct regs *regs)
{
    struct crash_entry *crash;

    crash = crash_find(addr);
    if (!crash)
        return CRASH_TYPE_NONE;

    pr_alert("Kenrel Crash at %s:%u in %s\n",
        crash->file, crash->line, crash->func);

    return crash->type;
}
