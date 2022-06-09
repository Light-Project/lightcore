/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <initcall.h>
#include <console.h>
#include <asm-generic/header.h>

static void blcon_write(struct console *con, const char *str, unsigned len)
{
    ((void (*)(const char *))(size_t)boot_head.stdout)(str);
}

static struct console_ops boot_console_ops = {
    .write = blcon_write,
};

static struct console boot_console = {
    .name = "bootcon",
    .ops = &boot_console_ops,
    .flags = CONSOLE_BOOT | CONSOLE_ENABLED,
};

static state boot_console_init(void)
{
    if (boot_head.stdout)
        earlycon_register(&boot_console);
    return -ENOERR;
}
pconsole_initcall(boot_console_init);
