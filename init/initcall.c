/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#define MUDOLE_NAME "init"
#define pr_fmt(fmt) MUDOLE_NAME ": " fmt

#include <kernel.h>
#include <string.h>
#include <init.h>
#include <param.h>
#include <initcall.h>
#include <printk.h>

char boot_args[BOOT_PARAM_SIZE];

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
    if ((ret = call()))
        pr_err("%s init failed, error code [%d]\n", fn->name, ret);
}

static void __init initcall_level(int level)
{
    initcall_entry_t *fn;

    pr_info("call level%d\n", level);

    for (fn = initcall_levels[level];
         fn < initcall_levels[level + 1];
         fn++) {
        do_one_initcall(fn);
    }
}

void __init initcalls(void)
{
    unsigned int level;

    for (level = 0;
         level < ARRAY_SIZE(initcall_levels) - 1;
         level++) {
        initcall_level(level);
    }
}

void __init ctors_initcall(void)
{
    ctor_initcall_t *ctor;

    for (ctor = _ld_ctors_start;
         ctor < _ld_ctors_end; ctor++)
        (*ctor)();
}

static state __init bootargs_entry(char *param, char *val, void *pdata)
{
    struct bootarg *arg;

    for (arg = _ld_boot_param_start;
         arg < _ld_boot_param_end; arg++) {
        if ((bool)pdata != arg->early)
            continue;
        if (!strcmp(param, arg->args))
            return arg->fn(val);
    }

    return -ENOENT;
}

void __init earlyargs_init(const char *cmd)
{
    char args_buff[BOOT_PARAM_SIZE];
    strncpy(args_buff, boot_args, BOOT_PARAM_SIZE);
    param_parser(args_buff, bootargs_entry, (void *)true);
}

void __init bootargs_init(const char *cmd)
{
    char args_buff[BOOT_PARAM_SIZE];
    strncpy(args_buff, boot_args, BOOT_PARAM_SIZE);
    param_parser(args_buff, bootargs_entry, (void *)false);
}
