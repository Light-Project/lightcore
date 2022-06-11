/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _LIBCPIO_H_
#define _LIBCPIO_H_

#include <types.h>

#define CPIO_NAME_MAX 16

struct cpio_data {
    char name[CPIO_NAME_MAX];
    const void *data;
    size_t size;
};

extern state cpio_data_find(struct cpio_data *cdata, const void *data, size_t length, const char *path, intptr_t *offset);

#endif  /* _LIBCPIO_H_ */
