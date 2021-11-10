/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <boot.h>

void (*stdout)(const char *) = NULL;
void __weak __noreturn panic_reboot(void) {while(1);}

int printf(const char *str, ...)
{
    char strbuf[128];
    va_list para;
    int len;

    if (!stdout)
        return 0;

    va_start(para,str);
    len = vsprintf(strbuf, str, para);
    va_end(para);
    stdout(strbuf);

    return len;
}

int pr_boot(const char *str, ...)
{
    char strbuf[128];
    va_list para;
    int len;

    if (!stdout)
        return 0;

    stdout("[pload]: ");
    va_start(para,str);
    len = vsprintf(strbuf, str, para);
    va_end(para);
    stdout(strbuf);

    return len;
}

void __noreturn panic(const char *str, ...)
{
    char strbuf[128];
    va_list para;

    if (!stdout)
        goto exit;

    stdout("[panic]: ");
    va_start(para,str);
    vsprintf(strbuf, str, para);
    va_end(para);
    stdout(strbuf);

exit:
    panic_reboot();
}

void pr_init(void (*fun)(const char *))
{
    stdout = fun;
    stdout("                   \n");
    stdout("-------------------\n");
    stdout("Lightcore preload! \n");
    stdout("-------------------\n");
}
