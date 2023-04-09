/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2022 John Sanpe <sanpeqf@gmail.com>
 */

#include <cmdline.h>
#include <kernel.h>
#include <kmalloc.h>
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

/**
 * levenshtein - Calculation string levenshtein edit distance.
 * @str1: string1 to calculation distance.
 * @str2: string2 to calculation distance.
 * @len1: the length of @str1, which is calculated internally if it is zero.
 * @len2: the length of @str2, which is calculated internally if it is zero.
 * @s: substitution distance reward.
 * @w: swap distance reward.
 * @a: addition distance reward.
 * @d: deletion distance reward.
 */
unsigned int levenshtein(const char *str1, const char *str2,
                         size_t len1, size_t len2, gfp_t gfp,
                         unsigned int s, unsigned int w,
                         unsigned int a, unsigned int d)
{
    unsigned int *row1, *row2, *row3;
    unsigned int distance, *cache;
    size_t index1, index2;

    if (!len1)
        len1 = strlen(str1);

    if (!len2)
        len2 = strlen(str2);

    if (unlikely(!len1))
        return len2 * a;

    if (unlikely(!len2))
        return len1 * a;

    if (len1 > len2) {
        swap(str1, str2);
        swap(len1, len2);
    }

    cache = kmalloc(BYTES_PER_INT * (len1 + 1) * 3, gfp);
    if (unlikely(!cache))
        return UINT_MAX;

    row1 = cache;
    row2 = row1 + (len1 + 1);
    row3 = row2 + (len1 + 1);

    for (index1 = 0; index1 < len1; ++index1)
        row2[index1] = index1 * a;

    for (index1 = 0; index1 < len1; ++index1) {
        row3[0] = (index1 + 1) * d;

        for (index2 = 0; index2 < len1; ++index2) {
            /* substitution distance reward */
            row3[index2 + 1] = row2[index2] + s * (str1[index1] != str2[index2]);

            /* swap distance reward */
            if (index1 > 0 && index2 > 0 && str1[index1 - 1] == str2[index2] &&
                str1[index1] == str2[index2 - 1] && row3[index2 + 1] > row1[index2 - 1] + w)
                row3[index2 + 1] = row1[index2 - 1] + w;

            /* deletion distance reward */
            if (row3[index2 + 1] > row2[index2 + 1] + d)
                row3[index2 + 1] = row2[index2 + 1] + d;

            /* addition distance reward */
            if (row3[index2 + 1] > row3[index2] + a)
                row3[index2 + 1] = row3[index2] + a;
        }

        swap(row1, row2);
        swap(row2, row3);
    }

    distance = row2[len1];
    kfree(cache);

    return distance;
}
EXPORT_SYMBOL(levenshtein);
