/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <initcall.h>
#include <driver/power.h>
#include <asm/proc.h>
#include <kshell.h>

static void help(void)
{
    printk("usage: reboot [OPTION]...       \n");
    printk("                                \n");
    printk("Reboot the system.              \n");
    printk("                                \n");
    printk("Options:                        \n");
    printk("  -h Show this help             \n");
    printk("  -H Halt the machine           \n");
    printk("  -S Switch off the machine     \n");
    printk("  -r Restart the machine        \n");
    printk("  -R Reboot the machine         \n");
}

static int bench_main(int argc, char *argv[])
{
    char *para;

    while (--argc) {
        para = *++argv;
        if (*para == '-' && para[1])
        switch (*++para) {
            case 'H':
                proc_halt();
            case 'S':
                power_shutdown();
            case 'r':
                power_restart();
            case 'R':
                power_reset();
            case 'h': default:
                help();
                return 0;
        }
    }

    power_restart();
    return 0;
}

static struct kshell_command bench_cmd = {
    .entry = bench_main,
    .name = "bench",
    .help = "BenchMark"
};

static state bench_init(void)
{
    return kshell_register(&bench_cmd);
}
kshell_initcall(bench_init);
