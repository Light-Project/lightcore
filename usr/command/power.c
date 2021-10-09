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

static int reset_main(int argc, char *argv[])
{
    char *para;

    while (--argc) {
        para = *++argv;
        if (*para == '-' && para[1])
        switch (*++para) {
            case 'H':
                cpu_halt();
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

static struct kshell_command reset_cmd = {
    .entry = reset_main,
    .name = "reboot",
    .help = "Reboot the system"
};

static state reset_init(void)
{
    return kshell_register(&reset_cmd);
}
kshell_initcall(reset_init);
