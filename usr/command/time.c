/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2022 Sanpe <sanpeqf@gmail.com>
 */

#include <kshell.h>
#include <initcall.h>
#include <timekeeping.h>

static state time_main(int argc, char *argv[])
{
    ktime_t start = timekeeping_get_time();
    kshell_execv(argv[1], argc - 1, &argv[1]);
    kshell_printf("total %llu.%03llums exec \"%s\"\n",
        ktime_to_ms(ktime_sub(timekeeping_get_time(), start)),
        ktime_to_us(ktime_sub(timekeeping_get_time(), start)) % 1000 / 10, argv[1]);
    return -ENOERR;
}

static struct kshell_command time_cmd = {
    .name = "time",
    .desc = "show process run time",
    .exec = time_main,
};

static state time_init(void)
{
    return kshell_register(&time_cmd);
}
kshell_initcall(time_init);
