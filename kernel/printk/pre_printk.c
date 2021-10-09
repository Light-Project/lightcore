/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <printk.h>
#include <console.h>

static struct console *pre_console;

void pre_console_write(const char *str, unsigned int len)
{
    if(!pre_console)
        return;
    pre_console->ops->write(pre_console, str, len);
}

int pr_early(const char *fmt, ...)
{
    char buf[256];
    int len;
    va_list ap;

    va_start(ap, fmt);
    len = vsnprintf(buf, sizeof(buf), fmt, ap);
    va_end(ap);

    pre_console_write(buf, len);
    return len;
}

void pre_console_register(struct console *con)
{
    if (!pre_console)
        pre_console = con;
}

void __weak pre_printk_init(void)
{
}
