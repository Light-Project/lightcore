/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <panic.h>
#include <export.h>
#include <printk.h>
#include <asm/proc.h>
#include <asm/irq.h>

static LIST_HEAD(panic_works_list);

static void panic_works(void)
{
    struct panic_work *work;
    const void *data;

    list_for_each_entry(work, &panic_works_list, list) {
        data = work->data;
        work->work(data);
    }
}

state panic_work_register(struct panic_work *work)
{
    if (!work->work)
        return -EINVAL;

    list_add(&panic_works_list, &work->list);
    return -ENOERR;
}

void __noreturn panic(const char* fmt, ...)
{
    char buf[256];
    va_list args;

    cpu_irq_disable();

    va_start(args, fmt);
    vsnprintf(buf, sizeof(buf), fmt, args);
    va_end(args);

    printk("Kernel Panic: %s\n", buf);
    printk("---[ end Kernel panic ]---\n");

    panic_works();
    proc_halt();
}
EXPORT_SYMBOL(panic);
