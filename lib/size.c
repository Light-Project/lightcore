/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <kernel.h>

const char *unit[] = {
    "Byte",
    "KiB", "MiB", "GiB",
    "TiB", "PiB", "EiB",
};

char *gsize(char *buff, double size)
{
    int count = 0;

    while ((size > 1024) && (count < ARRAY_SIZE(unit))) {
        size /= 1024;
        count++;
    }

    sprintf(buff, "%.2f%s", size, unit[count]);
    return buff;
}

