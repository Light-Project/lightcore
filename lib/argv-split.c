/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2022 Sanpe <sanpeqf@gmail.com>
 */

#include <argv-split.h>
#include <kmalloc.h>
#include <string.h>
#include <export.h>

unsigned int argv_count(const char *string)
{
    unsigned int argc = 1;

    while ((string = strchr(string, ' '))) {
        string = skip_spaces(string);
        argc++;
    }

    return argc;
}
EXPORT_SYMBOL(argv_count);

char **argv_split(const char *string, unsigned int *argcp, gfp_t flags)
{
    unsigned int count, argc;
    char *block, **argv;

    argc = argv_count(string);
    count = (argc + 1) * sizeof(*argv);

    argv = kmalloc(count + strlen(string) + 1, flags);
    if (unlikely(!argv))
        return NULL;

    block = (void *)argv + count;
    strcpy(block, string);

    for (count = 0; count < argc - 1; ++count) {
        argv[count] = block;
        block = strchr(block, ' ');
        *block++ = '\0';
        block = skip_spaces(block);
    }

    argv[count++] = block;
    argv[count] = NULL;

    if (argcp)
        *argcp = argc;

    return argv;
}
EXPORT_SYMBOL(argv_split);
