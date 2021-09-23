/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <kboot.h>
#include <types.h>
#include <stdarg.h>

void (*stdout)(const char *);
void __weak halt(void){}

int printf(const char *str, ...)
{
    char strbuf[128];
    va_list para;
    int len;
    
    if(!stdout)
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
    
    if(!stdout)
        return 0;
    
    stdout("[kboot]: ");    
    va_start(para,str);
    len = vsprintf(strbuf, str, para);
    va_end(para);
    stdout(strbuf);
    
    return len;
}

void panic(const char *str, ...)
{
    char strbuf[128];
    va_list para;
    
    if(!stdout)
        goto out;
    
    stdout("[panic]: ");    
    va_start(para,str);
    vsprintf(strbuf, str, para);
    va_end(para);
    stdout(strbuf);
    
out:
    halt();
} 

void pr_init(void (*fun)(const char *))
{
    stdout = fun;
    stdout("-----------------\n");
    stdout("Lightcore kboot!\n");
    stdout("-----------------\n");
}
