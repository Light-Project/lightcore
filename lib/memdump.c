/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2022 Sanpe <sanpeqf@gmail.com>
 */

#include <ctype.h>
#include <kernel.h>
#include <proc.h>
#include <memdump.h>
#include <printk.h>
#include <export.h>

static int dec_align_table[] = {
    [0] = 3, [1] = 5, [2] = 11, [4] = 20,
};

static int oct_align_table[] = {
    [0] = 3, [1] = 6, [2] = 11, [4] = 22,
};

int memdump_buffer(char *buffer, size_t buffsize, const void *data, size_t data_len,
                   int rowsize, int groupsize, int base, bool ascii)
{
    const char *str = data;
    char fmtbuff[16];
    unsigned int count, ngroup;
    int retval, length = 0;
    char ch, fmt;

    if (!is_power_of_2(groupsize) || groupsize > 8)
        groupsize = 1;

    if (data_len % groupsize)
        groupsize = 1;

    rowsize = align_high(rowsize, groupsize);
    data_len = min(data_len, rowsize);
    ngroup = data_len / groupsize;

    if (!data_len)
        goto finish;

    if (!buffsize)
        goto overflow1;

    switch (base) {
        case 8:
            fmt = 'o';
            break;

        case 10:
            fmt = 'u';
            break;

        case 16: default:
            fmt = 'x';
            break;
    }

    if (fmt == 'x') {
        sprintf(fmtbuff, "%%s%%#0%d%s%c", groupsize * 2 + 2,
                groupsize == 8 ? "ll" : "", fmt);
    } else {
        sprintf(fmtbuff, "%%s%%-%d%s%c", (fmt == 'o' ?
                oct_align_table : dec_align_table)[groupsize / 2],
                groupsize == 8 ? "ll" : "", fmt);
    }

    switch (groupsize) {
        case 1: default:
            for (count = 0; count < ngroup; ++count) {
                retval = snprintf(buffer + length, buffsize - length, fmtbuff, count ? "  " : "",
                                  *(uint8_t *)(data + count * groupsize));
                if (retval >= buffsize - length)
                    goto overflow1;
                length += retval;
            }
            break;

        case 2:
            for (count = 0; count < ngroup; ++count) {
                retval = snprintf(buffer + length, buffsize - length, fmtbuff, count ? "  " : "",
                                  unaligned_get_u16(data + count * groupsize));
                if (retval >= buffsize - length)
                    goto overflow1;
                length += retval;
            }
            break;

        case 4:
            for (count = 0; count < ngroup; ++count) {
                retval = snprintf(buffer + length, buffsize - length, fmtbuff, count ? "  " : "",
                                  unaligned_get_u32(data + count * groupsize));
                if (retval >= buffsize - length)
                    goto overflow1;
                length += retval;
            }
            break;

        case 8:
            for (count = 0; count < ngroup; ++count) {
                retval = snprintf(buffer + length, buffsize - length, fmtbuff, count ? "  " : "",
                                  unaligned_get_u64(data + count * groupsize));
                if (retval >= buffsize - length)
                    goto overflow1;
                length += retval;
            }
            break;
    }

    if (ascii) {
        if (buffsize < length + 2)
            goto overflow1;

        buffer[length++] = ' ';

        for (count = 0; count < data_len; ++count) {
            if (buffsize < length + 2)
                goto overflow2;
            ch = str[count];
            buffer[length++] = (isascii(ch) && isprint(ch)) ? ch : '-';
        }
    }

    buffer[length] = '\0';

finish:
    return length;

overflow2:
    buffer[length] = '\0';

overflow1:
    switch (base) {
        case 8: case 10:
            count = (base == 8 ? oct_align_table : dec_align_table)[groupsize / 2];
            break;

        case 16: default:
            count = (groupsize * 2) + 2;
            break;
    }

    count += 2;
    count *= ngroup;

    return ascii ? count + data_len : count;
}
EXPORT_SYMBOL(memdump_buffer);

int memdump_print(memdump_print_t func, void *pdata, const char *prefix, enum memdump_prefix_mode mode,
                  const void *data, size_t data_len, int rowsize, int groupsize, int base, bool ascii)
{
    char linebuf[128];
    unsigned int linelen;
    uintptr_t offset;
    int length;

    rowsize = align_high(rowsize, groupsize);
    for (length = 0, offset = 0; (linelen = min(rowsize, data_len)); data_len -= rowsize, offset += rowsize) {
        memdump_buffer(linebuf, sizeof(linebuf), data + offset, linelen, rowsize, groupsize, base, ascii);

        switch (mode) {
            case MEMDUMP_PREFIX_NONE: default:
                length += func(pdata, "%s%s\n", prefix, linebuf);
                break;

            case MEMDUMP_PREFIX_OFFSET:
                length += func(pdata, "%s%#.8lx: %s\n", prefix, offset, linebuf);
                break;

            case MEMDUMP_PREFIX_ADDRESS:
                length += func(pdata, "%s%p: %s\n", prefix, data + offset, linebuf);
                break;
        }
    }

    return length;
}
EXPORT_SYMBOL(memdump_print);

int memdump_printk(const char *level, const char *prefix, enum memdump_prefix_mode mode,
                   const void *data, size_t data_len, int rowsize, int groupsize, int base, bool ascii)
{
    char linebuf[128];
    unsigned int linelen;
    uintptr_t offset;
    int length;

    rowsize = align_high(rowsize, groupsize);
    for (length = 0, offset = 0; (linelen = min(rowsize, data_len)); data_len -= rowsize, offset += rowsize) {
        memdump_buffer(linebuf, sizeof(linebuf), data + offset, linelen, rowsize, groupsize, base, ascii);

        switch (mode) {
            case MEMDUMP_PREFIX_NONE: default:
                length += printk("%s%s%s\n", level, prefix, linebuf);
                break;

            case MEMDUMP_PREFIX_OFFSET:
                length += printk("%s%s%#.8lx: %s\n", level, prefix, offset, linebuf);
                break;

            case MEMDUMP_PREFIX_ADDRESS:
                length += printk("%s%s%p: %s\n", level, prefix, data + offset, linebuf);
                break;
        }
    }

    return length;
}
EXPORT_SYMBOL(memdump_printk);
