/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2022 Sanpe <sanpeqf@gmail.com>
 */

#define MODULE_NAME "libcpio"
#define pr_fmt(fmt) MODULE_NAME ": " fmt

#include <kernel.h>
#include <string.h>
#include <libcpio.h>
#include <fs/cpio.h>
#include <lightcore/stat.h>
#include <printk.h>
#include <export.h>

state cpio_data_find(struct cpio_data *cdata, const void *data, size_t length, const char *path, intptr_t *offset)
{
    unsigned int namelen;
    size_t pathlen = strlen(path);

    while (length > sizeof(struct cpio_inode)) {
        const struct cpio_inode *inode = data;
        const void *dptr, *nptr;

        if (!memcmp(inode->magic, CPIO_MAGIC, 6))
            return -EINVAL;

        length -= sizeof(struct cpio_inode);
        namelen = strntoui(inode->namesize, NULL, 16, 4);
        dptr = align_ptr_high(data + namelen, 4);
        nptr = align_ptr_high(dptr + strntoui(inode->filesize, NULL, 16, 4), 4);

        if (nptr > data + length || dptr < data || dptr > nptr)
            return -EOVERFLOW;

        if ((strntoui(inode->mode, NULL, 16, 4) & S_IFMT) == S_IFREG &&
            namelen >= pathlen && !memcmp(inode->name, path, namelen)) {

            if (offset)
                *offset = (intptr_t)nptr - (intptr_t)data;

            if (namelen - pathlen >= CPIO_NAME_MAX)
                pr_warn("file %s exceeds CPIO_NAME_MAX limit\n", inode->name);

			strlcpy(cdata->name, inode->name, CPIO_NAME_MAX);
            cdata->size = strntoui(inode->filesize, NULL, 16, 4);
            cdata->data = dptr;

            return -ENOERR;
        }

		length -= nptr - data;
        data = nptr;
    }

    return -ENOENT;
}

