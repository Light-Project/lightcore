/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2022 Sanpe <sanpeqf@gmail.com>
 */

#include <cmdline.h>
#include <kernel.h>
#include <export.h>

uint64_t memparse(const char *ptr, char **endptr)
{
    uint64_t value;
    char *alpha;

    value = strtou64(ptr, &alpha, 0);
    switch (*alpha) {
        case 'E': case 'e':
            value <<= 10;
            fallthrough;

        case 'P': case 'p':
            value <<= 10;
            fallthrough;

        case 'T': case 't':
            value <<= 10;
            fallthrough;

        case 'G': case 'g':
            value <<= 10;
            fallthrough;

        case 'M': case 'm':
            value <<= 10;
            fallthrough;

        case 'K': case 'k':
            value <<= 10;
            alpha++;
            fallthrough;

        default:
            break;
    }

    if (endptr)
        *endptr = alpha;

    return value;
}
EXPORT_SYMBOL(memparse);
