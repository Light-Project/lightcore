/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _ERROR_H_
#define _ERROR_H_

#include <types.h>
#include <state.h>

/**
 * ERR_PTR - convert state to pointer
 */
static inline void *ERR_PTR(state err)
{
    return (void *)(ssize_t)err;
}

/**
 * PTR_ERR - convert pointer to state
 */
static inline state PTR_ERR(void *ptr)
{
    if (unlikely((state)(ssize_t)ptr >= -ERRNO_MAX))
        return (state)(ssize_t)ptr;
    return -ENOERR;
}

#endif  /* _ERROR_H_ */
