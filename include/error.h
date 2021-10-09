/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _ERROR_H_
#define _ERROR_H_

#include <types.h>
#include <state.h>

static inline void *ERR_PTR(state err)
{
    return (void *)err;
}

static inline state PTR_ERR(void *ptr)
{
    if (unlikely((state)ptr >= -ERRNO_MAX))
        return (state)ptr;
    return -ENOERR;
}

#endif  /* _ERROR_H_ */
