/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2022 Sanpe <sanpeqf@gmail.com>
 */

#include <ctype.h>
#include <kernel.h>
#include <proc.h>
#include <printk.h>
#include <fmtdump.h>
#include <export.h>

static int dec_align_table[] = {
    [0] = 3, [1] = 5, [2] = 11, [4] = 20,
};

static int oct_align_table[] = {
    [0] = 3, [1] = 6, [2] = 11, [4] = 22,
};

int fmtdump_line(const struct fmtdump_ops *ops, void *pdata, char *buff, size_t buffsize,
                 uintptr_t addr, size_t size, int rowsize, int groupsize, int base, bool ascii)
{
    char fmtbuff[16];
    unsigned int count, ngroup;
    int retval, length = 0;
    char ch, fmt;

    if (!is_power_of_2(groupsize) || groupsize > 8)
        groupsize = 1;

    if (size % groupsize)
        groupsize = 1;

    rowsize = align_high(rowsize, groupsize);
    size = min(size, rowsize);
    ngroup = size / groupsize;

    if (!size)
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
                retval = snprintf(buff + length, buffsize - length, fmtbuff, count ? " " : "",
                                  ops->read8(pdata, addr + count * groupsize));
                if (retval >= buffsize - length)
                    goto overflow1;
                length += retval;
            }
            break;

        case 2:
            for (count = 0; count < ngroup; ++count) {
                retval = snprintf(buff + length, buffsize - length, fmtbuff, count ? " " : "",
                                  ops->read16(pdata, addr + count * groupsize));
                if (retval >= buffsize - length)
                    goto overflow1;
                length += retval;
            }
            break;

        case 4:
            for (count = 0; count < ngroup; ++count) {
                retval = snprintf(buff + length, buffsize - length, fmtbuff, count ? " " : "",
                                  ops->read32(pdata, addr + count * groupsize));
                if (retval >= buffsize - length)
                    goto overflow1;
                length += retval;
            }
            break;

        case 8:
            for (count = 0; count < ngroup; ++count) {
                retval = snprintf(buff + length, buffsize - length, fmtbuff, count ? " " : "",
                                  ops->read64(pdata, addr + count * groupsize));
                if (retval >= buffsize - length)
                    goto overflow1;
                length += retval;
            }
            break;
    }

    if (ascii) {
        if (buffsize < length + 2)
            goto overflow1;

        buff[length++] = ' ';
        buff[length++] = ' ';

        for (count = 0; count < size; ++count) {
            if (buffsize < length + 2)
                goto overflow2;
            ch = ops->read8(pdata, addr + count);
            buff[length++] = (isascii(ch) && isprint(ch)) ? ch : '.';
        }
    }

    buff[length] = '\0';

finish:
    return length;

overflow2:
    buff[length] = '\0';

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

    return ascii ? count + size : count;
}
EXPORT_SYMBOL(fmtdump_line);

int fmtdump_print(const struct fmtdump_ops *ops, void *pdata, const char *prefix, enum fmtdump_prefix_mode mode,
                  uintptr_t addr, size_t size, int rowsize, int groupsize, int base, bool ascii)
{
    char linebuff[128];
    unsigned int linelen;
    uintptr_t offset;
    int retval;

    rowsize = align_high(rowsize, groupsize);
    for (retval = 0, offset = 0; (linelen = min(rowsize, size)); size -= rowsize, offset += rowsize) {
        fmtdump_line(ops, pdata, linebuff, sizeof(linebuff), addr + offset, linelen, rowsize, groupsize, base, ascii);

        switch (mode) {
            case FMTDUMP_PREFIX_NONE: default:
                retval += ops->print(pdata, "%s%s\n", prefix, linebuff);
                break;

            case FMTDUMP_PREFIX_OFFSET:
                retval += ops->print(pdata, "%s%#.8lx: %s\n", prefix, offset, linebuff);
                break;

            case FMTDUMP_PREFIX_ADDRESS:
                retval += ops->print(pdata, "%s%p: %s\n", prefix, addr + offset, linebuff);
                break;
        }
    }

    return retval;
}
EXPORT_SYMBOL(fmtdump_print);
