/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <gsize.h>
#include <kernel.h>
#include <export.h>

const char *unit[] = {
    "Byte",
    "KiB", "MiB", "GiB",
    "TiB", "PiB", "EiB",
};

int gsize(char *buff, size_t size)
{
    unsigned int count = 0;
    size_t prev = 0;

    while ((size >= 1024) && (count <= ARRAY_SIZE(unit))) {
        size = (prev = size) >> 10; /* prev / 1024 */
        ++count;
    }

    return scnprintf(buff, GSIZE_BUFF, "%ld.%02ld%s",
                     size, (prev & 1023) / 10, unit[count]);
}
EXPORT_SYMBOL(gsize);
