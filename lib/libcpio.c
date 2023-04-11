/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2022 John Sanpe <sanpeqf@gmail.com>
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

state cpio_lookup(struct cpio_data *cdata, const void *data, size_t length, const char *path, intptr_t *offset)
{
    const void * const start = data;
    size_t pathlen = strlen(path);
    unsigned int namelen;

    while (length > sizeof(struct cpio_inode)) {
        const struct cpio_inode *inode = data;
        const void *dptr, *nptr;

        if (memcmp(inode->magic, CPIO_MAGIC, 6))
            return -EINVAL;

        length -= sizeof(struct cpio_inode);
        data += sizeof(struct cpio_inode);

        namelen = strntoui(inode->namesize, NULL, 16, 8);
        dptr = align_ptr_high(data + namelen, 4);
        nptr = align_ptr_high(dptr + strntoui(inode->filesize, NULL, 16, 8), 4);

        if (nptr > data + length || dptr < data || dptr > nptr)
            return -EOVERFLOW;

        if (unlikely(namelen == sizeof(CPIO_TRAILER_NAME) &&
            !memcmp(CPIO_TRAILER_NAME, inode->name, namelen)))
            return -ENOENT;

        if ((strntoui(inode->mode, NULL, 16, 8) & S_IFMT) == S_IFREG &&
            namelen >= pathlen && !memcmp(inode->name, path, pathlen)) {

            if (offset)
                *offset = (intptr_t)nptr - (intptr_t)start;

            if (namelen - pathlen >= CPIO_NAME_MAX)
                pr_warn("file %s exceeds CPIO_NAME_MAX limit\n", inode->name);

            strlcpy(cdata->name, inode->name, CPIO_NAME_MAX);
            cdata->size = strntoui(inode->filesize, NULL, 16, 8);
            cdata->data = dptr;

            return -ENOERR;
        }

        length -= nptr - data;
        data = nptr;
    }

    return -ENOENT;
}
EXPORT_SYMBOL(cpio_lookup);
