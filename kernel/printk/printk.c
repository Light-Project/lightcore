/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <printk.h>
#include <string.h>
#include <kernel.h>
#include <console.h>
#include <timekeeping.h>
#include <export.h>

static inline char printk_get_level(const char *str)
{
    if (str[0] == KERN_SOH_ASCII && str[1])
        return str[1];
    return 0;
}

static const char *printk_level(const char *str, enum klevel *klevel)
{
    char kern_level;

    for (*klevel = KLEVEL_DEFAULT; *str; str += 2) {
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

static inline int printk_time(char *buffer, struct timespec *time)
{
    return sprintf(buffer, "[%5llu.%06lu] ",
        time->tv_sec, time->tv_nsec / 1000);
}

int vprintk(const char *fmt, va_list args)
{
    char buffer[256], *p = buffer;
    enum klevel klevel;
    int len;

    fmt = printk_level(fmt, &klevel);

#ifdef CONFIG_PRINTK_TIME
    struct timespec btime;
    btime = timekeeping_get_time_ts();
    if (strchr(fmt, '\n'))
        p += printk_time(p, &btime);
#endif

    p += vsprintf(p, fmt, args);
    len = p - buffer;

    console_write(buffer, len);
    return len;
}
EXPORT_SYMBOL(vprintk);

int printk(const char *fmt, ...)
{
    va_list para;
    int length;

    va_start(para,fmt);
    length = vprintk(fmt, para);
    va_end(para);

    return length;
}
EXPORT_SYMBOL(printk);
