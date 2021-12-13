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

char *gsize(char *buff, size_t size)
{
    unsigned int count = 0;
    size_t prev;

    while ((size > 1024) && (count < ARRAY_SIZE(unit))) {
        size = (prev = size) / 1024;
        count++;
    }

    sprintf(buff, "%ld.%02ld%s", size, prev % 1024, unit[count]);
    return buff;
}

