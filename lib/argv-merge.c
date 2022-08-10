/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2022 Sanpe <sanpeqf@gmail.com>
 */

#include <kernel.h>
#include <argv-split.h>
#include <export.h>

int argv_merge(int argc, char *argv[], char *buff, int size)
{
    #define argv_sprintf(fmt, ...) len += snprintf(buff + len, max(0, size - len), fmt, ##__VA_ARGS__)
    int count, len = 0;

    for (count = 0; count < argc - 1; ++count)
        argv_sprintf("%s ", argv[count]);
    argv_sprintf("%s", argv[count]);

    return len + 1;
}
