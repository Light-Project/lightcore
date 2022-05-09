/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <kshell.h>
#include <initcall.h>
#include <power.h>

static void usage(struct kshell_context *ctx)
{
    kshell_printf(ctx, "usage: reboot [option] ...\n");
    kshell_printf(ctx, "\t-H  halt the machine\n");
    kshell_printf(ctx, "\t-S  switch off the machine\n");
    kshell_printf(ctx, "\t-R  cold restart the machine\n");
    kshell_printf(ctx, "\t-r  warn restart the machine\n");
    kshell_printf(ctx, "\t-s  soft restart the machine\n");
    kshell_printf(ctx, "\t-f  hard restart the machine\n");
    kshell_printf(ctx, "\t-h  display this message\n");
}

static state reset_main(struct kshell_context *ctx, int argc, char *argv[])
{
    const char *para;

    while (--argc) {
        para = *++argv;
        if (*para == '-' && para[1]) {
            switch (*++para) {
                case 'H':
                    kernel_halt();

                case 'S':
                    kernel_poweroff();

                case 'R':
                    kernel_restart(POWER_RESTART_COLD);

                case 'r':
                    kernel_restart(POWER_RESTART_WARM);

                case 's':
                    kernel_restart(POWER_RESTART_SOFT);

                case 'f':
                    kernel_restart(POWER_RESTART_HARD);

                case 'h': default:
                    usage(ctx);
                    return -ENOERR;
            }
        }
    }

    kernel_reboot();
    return -ENOERR;
}

static struct kshell_command reset_cmd = {
    .name = "reboot",
    .desc = "reboot the system",
    .exec = reset_main,
};

static state reset_init(void)
{
    return kshell_register(&reset_cmd);
}
kshell_initcall(reset_init);
