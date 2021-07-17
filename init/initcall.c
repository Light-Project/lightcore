/* SPDX-License-Identifier: GPL-2.0-or-later */
#define pr_fmt(fmt) "init: " fmt

#include <types.h>
#include <init/initcall.h>
#include <printk.h>

static initcall_entry_t *initcall_levels[] __initdata = {
    _ld_initcall_start,
    _ld_initcall1_start,
    _ld_initcall2_start,
    _ld_initcall3_start,
    _ld_initcall4_start,
    _ld_initcall5_start,
    _ld_initcall6_start,
    _ld_initcall7_start,
    _ld_initcall_end,
};

#ifdef CONFIG_DYNAMIC_DEBUG
static const char *initcall_level_names[] __initdata = {
    "pure",
    "core",
    "arch",
    "framework",
    "filesystem",
    "rootfs",
    "driver",
    "late",
};
#endif

static void __init do_one_initcall(int level, initcall_entry_t *fn)
{
    initcall_t call;
    state ret;

    pr_debug("[%s] starting %s...\n\r", initcall_level_names[level], fn->name);

    call = initcall_from_entry(fn);
    ret = call();

    char *stat = "OK";
    if(ret)
        stat = "FAIL";

    pr_debug("started %s %s. \n\r", fn->name, stat);
}

static void __init do_initcall_level(int level)
{
    initcall_entry_t *fn;

    for(fn = initcall_levels[level]; fn < initcall_levels[level+1]; fn++)
            do_one_initcall(level, fn);
}

void __init initcalls(void)
{
    int level;
    for (level = 0; level < ARRAY_SIZE(initcall_levels) - 1; level++) {
        do_initcall_level(level);
    }
}
