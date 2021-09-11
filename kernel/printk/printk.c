/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <kernel.h>
#include <console.h>
#include <time.h>
#include <export.h>
#include <printk.h>

static inline char printk_get_level(const char *str)
{
    if (str[0] == KERN_SOH_ASCII && str[1])
        return str[1];
    return 0;
}

static const char *printk_level(const char *str, enum klevel *klevel)
{
    char kern_level;

    for(*klevel = KLEVEL_DEFAULT; *str; str += 2) {
        kern_level = printk_get_level(str);
        if (!kern_level)
            break;

        switch (kern_level) {
            case '0' ... '9':
                *klevel = kern_level - '0';
        }
    }

    return str;
}

static int vprintk(const char *fmt, va_list args)
{
    char buffer[256];
    enum klevel klevel;
    int len;

    fmt = printk_level(fmt, &klevel);
    len = vsnprintf(buffer, sizeof(buffer), fmt, args);

    console_write(buffer, len);
    return len;
}

asmlinkage __visible int printk(const char *fmt, ...)
{
    va_list para;
    int length;

    va_start(para,fmt);
    length = vprintk(fmt, para);
    va_end(para);

    return length;
}
EXPORT_SYMBOL(printk);
