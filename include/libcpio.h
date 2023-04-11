/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _LIBCPIO_H_
#define _LIBCPIO_H_

#include <types.h>

#ifndef CPIO_NAME_MAX
# define CPIO_NAME_MAX 64
#endif

struct cpio_data {
    char name[CPIO_NAME_MAX];
    const void *data;
    size_t size;
};

extern state cpio_lookup(struct cpio_data *cdata, const void *data, size_t length,
                         const char *path, intptr_t *offset);

#define cpio_for_each(cdata, offset, retval, data, length, path) \
    for (*(retval) = cpio_lookup(cdata, data, length, path, offset); !*(retval); \
         data += *(offset), *(retval) = cpio_lookup(cdata, data, length, path, offset))

#endif /* _LIBCPIO_H_ */
