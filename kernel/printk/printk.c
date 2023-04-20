/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 John Sanpe <sanpeqf@gmail.com>
 */

#include <printk.h>
#include <string.h>
#include <kernel.h>
#include <console.h>
#include <initcall.h>
#include <kclock.h>
#include <export.h>

unsigned int printk_levels[PRIDX_NUM_MAX] = {
    [PRIDX_CONSOLE] = CONSOLE_LOGLEVEL_DEFAULT,
    [PRIDX_DEFAULT] = PRINTK_LOGLEVEL_DEFAULT,
    [PRIDX_RECORD]  = PRINTK_LOGLEVEL_RECORD,
};
EXPORT_SYMBOL(printk_levels);

const unsigned int printk_level_color[] = {
    [KLEVEL_EMERG]      = KERN_COLR_RED,
    [KLEVEL_ALERT]      = KERN_COLR_DARK_MAGENTA,
    [KLEVEL_CRIT]       = KERN_COLR_MAGENTA,
    [KLEVEL_ERR]        = KERN_COLR_YELLOW,
    [KLEVEL_WARNING]    = KERN_COLR_BLUE,
    [KLEVEL_NOTICE]     = KERN_COLR_CYAN,
    [KLEVEL_INFO]       = KERN_COLR_GREEN,
    [KLEVEL_DEBUG]      = KERN_COLR_DARK_GRAY,
    [KLEVEL_DEFAULT]    = KERN_COLR_DEFAULT,
};
EXPORT_SYMBOL(printk_level_color);

static state bootarg_recordlevel(char *args)
{
    printk_record = atoui(args);
    return -ENOERR;
}
earlyarg_initcall("recordlevel", bootarg_recordlevel);

static state bootarg_loglevel(char *args)
{
    printk_console = atoui(args);
    return -ENOERR;
}
earlyarg_initcall("loglevel", bootarg_loglevel);

static state bootarg_debug(char *args)
{
    printk_console = CONSOLE_LOGLEVEL_DEBUG;
    return -ENOERR;
}
earlyarg_initcall("debug", bootarg_debug);

static state bootarg_quiet(char *args)
{
    printk_console = CONSOLE_LOGLEVEL_QUIET;
    return -ENOERR;
}
earlyarg_initcall("quiet", bootarg_quiet);

static inline char printk_get_level(const char *str)
{
    if (str[0] == KERN_SOH_ASCII && str[1])
        return str[1];
    return 0;
}

unsigned int printk_level(const char *str, const char **endptr)
{
    char value, klevel;

    for (klevel = KLEVEL_DEFAULT; *str; str += 2) {
        value = printk_get_level(str);
        if (!value)
            break;

        switch (value) {
            case '0' ... '9':
                klevel = value - '0';
                break;

            default:
                break;
        }
    }

    if (*endptr)
        *endptr = str;

    return klevel;
}
EXPORT_SYMBOL(printk_level);

static inline int printk_time(char *buffer, struct timespec *time)
{
    return sprintf(buffer, "[%5llu.%06lu] ",
                   time->tv_sec, time->tv_nsec / 1000);
}

int vprintk(const char *fmt, va_list args)
{
    char buffer[256], *p = buffer;
    unsigned int klevel;
    int len;

    klevel = printk_level(fmt, &fmt);
    if (klevel == KLEVEL_DEFAULT)
        klevel = printk_default;

    if (klevel > printk_record)
        return 0;

#ifdef CONFIG_PRINTK_TIME
    struct timespec btime;
    btime = ns_to_timespec(kclock_get());
    if (strchr(fmt, '\n'))
        p += printk_time(p, &btime);
#endif

#ifdef CONFIG_PRINTK_COLOR
    p += sprintf(p, "\e[%dm", printk_level_color[klevel]);
#endif
    p += vsprintf(p, fmt, args);
#ifdef CONFIG_PRINTK_COLOR
    p += sprintf(p, "\e[0m");
#endif

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
