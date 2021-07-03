#include <types.h>
#include <stddef.h>
#include <printk.h>
#include <init/initcall.h>

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

static const char *initcall_level_names[] __initdata = {
    "pure",
    "core",
    "arch",
    "framework",
    "fs",
    "rootfs",
    "driver",
    "late",
};

int __init do_one_initcall(initcall_entry_t *fn)
{
    initcall_t call;
    call = initcall_from_entry(fn);
    return call();
}

void __init do_initcall_level(int level)
{
    initcall_entry_t *fn;
    
    printk("Initcall: %s\n\r", initcall_level_names[level]);
    
    for(fn = initcall_levels[level]; fn < initcall_levels[level+1]; fn++)
            do_one_initcall(fn);
}

void __init initcalls(void)
{
    int level;
    for (level = 0; level < ARRAY_SIZE(initcall_levels) - 1; level++) 
    {
        do_initcall_level(level);
    }
}
