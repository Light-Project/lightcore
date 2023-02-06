/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#define MODULE_NAME "clk"
#define pr_fmt(fmt) MODULE_NAME ": " fmt

#include <initcall.h>
#include <driver/clk.h>
#include <printk.h>

state clk_register(struct clk_device *clk)
{
    if (!clk->ops)
        return -EINVAL;

    return -ENOERR;
}

void clk_unregister(struct clk_device *clk)
{

}

void __init clk_init(void)
{
    initcall_entry_t *fn;
    initcall_t call;

    initcall_for_each_fn(fn, clk_initcall) {
        call = initcall_from_entry(fn);
        if (call())
            pr_err("%s startup failed", fn->name);
    }
}
