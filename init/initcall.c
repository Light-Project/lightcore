/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#define pr_fmt(fmt) "init: " fmt

#include <kernel.h>
#include <init.h>
#include <initcall.h>
#include <printk.h>

char boot_args[boot_args_size];

static __initdata initcall_entry_t *initcall_levels[] = {
    _ld_initcall_start,
    _ld_initcall1_start, _ld_initcall2_start,
    _ld_initcall3_start, _ld_initcall4_start,
    _ld_initcall5_start, _ld_initcall6_start,
    _ld_initcall7_start, _ld_initcall8_start,
    _ld_initcall_end,
};

static void __init do_one_initcall(initcall_entry_t *fn)
{
    initcall_t call;
    state ret;

    call = initcall_from_entry(fn);
    ret = call();

    if (ret)
        pr_err("%s error code [%d]\n", fn->name, ret);
}

static void __init initcall_level(int level)
{
    initcall_entry_t *fn;

    pr_info("call level%d\n", level);

    for (fn = initcall_levels[level];
         fn < initcall_levels[level+1]; fn++) {
        do_one_initcall(fn);
    }
}

void __init initcalls(void)
{
    int level;

    for (level = 0;
         level < ARRAY_SIZE(initcall_levels) - 1;
         level++) {
        initcall_level(level);
    }
}

void __init bootargs_init(const char *cmd)
{

}
