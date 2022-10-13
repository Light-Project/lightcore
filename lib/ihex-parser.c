/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#define MODULE_NAME "ihex-parser"
#define pr_fmt(fmt) MODULE_NAME ": " fmt

#include <ihex-parser.h>
#include <kernel.h>
#include <printk.h>
#include <export.h>

static bool ihex_checksum(const char *start, const char *checkum)
{
    uint8_t cumul;

    for (cumul = 0; start < checkum; start += 2)
        cumul += strntou8(start, NULL, 16, 2);

    cumul = 0x100 - cumul;
    return strntou8(checkum, NULL, 16, 2) == cumul;
}

state ihex_parser(const void *image, ihex_report_t report, void *pdata)
{
    const struct ihex_head *line;
    struct ihex_state ihex;
    char buff[255], *end;
    state retval;

    for (line = image; line; line = (void *)end + 1) {
        unsigned int length, type;
        unsigned int count;

        if (line->code == '#')
            continue;

        if (line->code != ':') {
            pr_err("error ihex format\n");
            return -EINVAL;
        }

        end = strchr(line->data, '\n');
        if (!end) {
            pr_err("eof without eof record\n");
            return -ENFILE;
        }

        /* Get ihex line information */
        length = strntou8(&line->length[0], NULL, 16, 2);
        type = strntou8(&line->type[0], NULL, 16, 2);

        /* Check the actual length of the data */
        if (((end - line->data - 2) / 2) != length) {
            pr_err("error data length\n");
            return -EFAULT;
        }

        if (!ihex_checksum((void *)line + 1, (void *)end - 2)) {
            pr_err("error data checksum\n");
            return -EFAULT;
        }

        switch (type) {
            case IHEX_TYPE_DATA: default:
                ihex.offset = strntou16(&line->offset[0], NULL, 16, 4);
                for (count = 0; count < length; ++count)
                    buff[count] = strntou8(&line->data[count * 2], NULL, 16, 2);
                break;

            case IHEX_TYPE_ESEG:
                if (length != 2) {
                    pr_err("invalid extended segment address\n");
                    return -ENODATA;
                }
                ihex.segm_exten = strntou16(&line->data[0], NULL, 16, 4);
                break;

            case IHEX_TYPE_SSEG:
                if (length != 4) {
                    pr_err("invalid start segment address\n");
                    return -ENODATA;
                }
                ihex.segm_start = strntou32(&line->data[0], NULL, 16, 8);
                break;

            case IHEX_TYPE_EADDR:
                if (length != 2) {
                    pr_err("invalid extended linear address\n");
                    return -ENODATA;
                }
                ihex.addr_exten = strntou16(&line->data[0], NULL, 16, 4);
                break;

            case IHEX_TYPE_SADDR:
                if (length != 4) {
                    pr_err("invalid extended linear address\n");
                    return -ENODATA;
                }
                ihex.addr_start = strntou32(&line->data[0], NULL, 16, 8);
                break;

            case IHEX_TYPE_EOF:
                return -ENOERR;
        }

        retval = report(&ihex, type, buff, length, pdata);
        if (unlikely(retval))
            return retval;
    }

    return -ENFILE;
}
EXPORT_SYMBOL(ihex_parser);
