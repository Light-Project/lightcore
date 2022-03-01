/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _ERROR_H_
#define _ERROR_H_

#include <types.h>
#include <state.h>

/**
 * ERR_PTR - converts a state to a pointer containing the state
 * @err: the state to convert
 */
static inline void *ERR_PTR(state err)
{
    return (void *)(ssize_t)err;
}

/**
 * PTR_ERR - converts a pointer containing an state to an state
 * @ptr: the pointer to convert
 */
static inline state PTR_ERR(void *ptr)
{
    if (unlikely((state)(ssize_t)ptr >= -ERRNO_MAX))
        return (state)(ssize_t)ptr;
    if (unlikely(!ptr))
        return -ENOMEM;
    return -ENOERR;
}

#endif  /* _ERROR_H_ */
