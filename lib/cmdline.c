/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2022 Sanpe <sanpeqf@gmail.com>
 */

#include <cmdline.h>
#include <kernel.h>
#include <ctype.h>
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

uint64_t timeparse(const char *ptr, char **endptr)
{
    uint64_t value;
    char *alpha;

    value = strtou64(ptr, &alpha, 0);
    switch (*alpha) {
        case 'S': case 's':
            value *= 1000;
            fallthrough;

        case 'M': case 'm':
            value *= 1000;
            fallthrough;

        case 'U': case 'u':
            value *= 1000;
            alpha++;
            fallthrough;

        default:
            break;
    }

    if (endptr)
        *endptr = alpha;

    return value;
}
EXPORT_SYMBOL(timeparse);

int escape_string(char *buff, size_t size, const char *fmt)
{
    char *str = buff;
    char ch, tmp;

    while ((ch = *fmt++) && size--) {
        if (ch != '\\') {
            *buff++ = ch;
            continue;
        }

        switch (*fmt++) {
            case 'a':
                *buff++ = '\a';
                break;

            case 'b':
                *buff++ = '\b';
                break;

            case 'e':
                *buff++ = '\e';
                break;

            case 'f':
                *buff++ = '\f';
                break;

            case 'n':
                *buff++ = '\n';
                break;

            case 'r':
                *buff++ = '\r';
                break;

            case 't':
                *buff++ = '\t';
                break;

            case 'v':
                *buff++ = '\v';
                break;

            case 'x':
                if (!isxdigit(tmp = *fmt++))
                    goto not_an_escape;

                *buff = chtoi(tmp);
                tmp = *fmt;

                if (isxdigit(tmp)) {
                    fmt++;
                    *buff = *buff * 16 + chtoi(tmp);
                }
                buff++;
                break;

            case '0':
                *buff++ = '\0';
                if (!('0' <= *fmt && *fmt <= '7'))
                    goto finish;
                *--buff = *fmt++;
                fallthrough;

            case '1': case '2': case '3':
            case '4': case '5': case '6': case '7':
                *buff = *(fmt - 1) - '0';
                if ('0' <= *fmt && *fmt <= '7')
                    *buff = *buff * 8 + (*fmt++ - '0');
                if ('0' <= *fmt && *fmt <= '7')
                    *buff = *buff * 8 + (*fmt++ - '0');
                buff++;
                break;

            default: not_an_escape:
                fmt--;
                fallthrough;

            case '\\':
                *buff++ = '\\';
                break;
        }
    }

finish:
    *buff++ = '\0';
    return buff - str;
}
EXPORT_SYMBOL(escape_string);
