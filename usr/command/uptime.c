/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <kshell.h>
#include <initcall.h>
#include <timekeeping.h>
#include <printk.h>

static state uptime_main(int argc, char *argv[])
{
    return -ENOERR;
}

static struct kshell_command uptime_cmd = {
    .name = "uptime",
    .desc = "show uptime",
    .exec = uptime_main,
};

static state uptime_init(void)
{
    return kshell_register(&uptime_cmd);
}
kshell_initcall(uptime_init);
