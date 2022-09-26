/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#define MUDOLE_NAME "init"
#define pr_fmt(fmt) MUDOLE_NAME ": " fmt

#include <kernel.h>
#include <string.h>
#include <slist.h>
#include <init.h>
#include <param.h>
#include <initcall.h>
#include <kmalloc.h>
#include <printk.h>
#include <panic.h>

struct blacklist_entry {
    struct slist_head list;
    char name[1];
};

char boot_args[BOOT_PARAM_SIZE];
static unsigned int initcall_time;
static SLIST_HEAD(blacklisted);

static __initdata initcall_entry_t *initcall_levels[] = {
    _ld_initcall_start,
    _ld_initcall1_start, _ld_initcall2_start,
    _ld_initcall3_start, _ld_initcall4_start,
    _ld_initcall5_start, _ld_initcall6_start,
    _ld_initcall7_start, _ld_initcall8_start,
    _ld_initcall_end,
};

static state initcall_retry(char *args)
{
    initcall_time = atoui(args);
    return -ENOERR;
}
bootarg_initcall("initcall_retry", initcall_retry);

static state initcall_blacklist(char *args)
{
    struct blacklist_entry *entry;
    char *ename;

    for (;;) {
        ename = strsep(&args, ",");
        if (!ename)
            break;

        pr_debug("blacklisting initcall %s\n", ename);
        entry = kmalloc(sizeof(*entry) + strlen(ename), GFP_KERNEL);
        if (BUG_ON(!entry))
            return -ENOMEM;

        strcpy(entry->name, ename);
        slist_add(&blacklisted, &entry->list);
    }

    return -ENOERR;
}
bootarg_initcall("initcall_blacklist", initcall_blacklist);

static bool initcall_blacklisted(const char *fname)
{
    struct blacklist_entry *entry;

    if (slist_check_empty(&blacklisted))
        return false;

    slist_for_each_entry(entry, &blacklisted, list) {
        if (!strcmp(fname, entry->name)) {
            pr_debug("initcall %s blacklisted\n", fname);
            return true;
        }
    }

    return false;
}

static state __init do_one_initcall(initcall_entry_t *fn)
{
    unsigned int count;
    unsigned long preempt;
    char msgbuff[64];
    initcall_t call;
    state retval;

    if (initcall_blacklisted(fn->name))
        return -EPERM;

    preempt = preempt_count();
    msgbuff[0] = 0;

    for (count = 0; count <= initcall_time; ++count) {
        call = initcall_from_entry(fn);
        if (!(retval = call()))
            break;
        pr_err("initcall %s failed, exit code [%d]\n", fn->name, retval);
    }

    if (preempt_count() != preempt) {
        raw_preempt_count_set(preempt);
        sprintf(msgbuff, "preemption imbalance ");
    }

    if (irq_local_disabled()) {
        irq_local_enable();
        sprintf(msgbuff, "interrupt changed ");
    }

    if (msgbuff[0]) {
        pr_crit("initcall %s leave mess %s\n", fn->name, msgbuff);
        WARN();
    }

    return retval;
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
