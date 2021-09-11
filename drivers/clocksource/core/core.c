/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#define pr_fmt(fmt) "clksrc: " fmt

#include <initcall.h>
#include <driver/clocksource.h>
#include <printk.h>

state clocksource_register(struct clocksource_device *cdev)
{
    return -ENOERR;
}

void timer_init(void)
{
    initcall_entry_t *fn;
    initcall_t call;

    initcall_for_each_fn(fn, clocksource_initcall) {
        call = initcall_from_entry(fn);
        if (call())
            pr_err("%s startup failed", fn->name);
    }
}
