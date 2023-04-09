/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 John Sanpe <sanpeqf@gmail.com>
 */

#include <console.h>
#include <ascii.h>
#include <driver/vt.h>

static void vt_console_write(struct console *console, const char *buff, unsigned int len)
{
    unsigned int count;
    char ch;

    for (count = 0; count < len; ++count) {
        ch = buff[count];

    }


}

static struct console_ops vt_console_ops = {
    .write = vt_console_write,
};
