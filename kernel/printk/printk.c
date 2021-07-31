/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <types.h>
#include <stdarg.h>
#include <stddef.h>
#include <printk.h>
#include <kfifo.h>
#include <console.h>
#include <init/initcall.h>
#include <kernel.h>

#define ringbuffer_len  0x4000

static char buffer[ringbuffer_len];
struct kfifo ringbuffer = {
    .buffer = buffer,
    .size = ringbuffer_len,
};

static void vprintk(char *str, int len)
{
    console_write(str, len);
}

int printk(const char *fmt,...)
{
    va_list para;
    char buf[128];
    int len;
    
    va_start(para,fmt);
    len = vsnprintf(buf, sizeof(buf), fmt, para);
    va_end(para);

    vprintk(buf, len);

    return len;   
}

