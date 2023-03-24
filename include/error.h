/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _ERROR_H_
#define _ERROR_H_

#include <types.h>
#include <state.h>

/**
 * ERR_PTR - converts a state to a pointer containing the state.
 * @err: the state to convert.
 */
static inline void *ERR_PTR(state err)
{
    return (void *)(intptr_t)err;
}

/**
 * PTR_ERR - converts a pointer containing an state to an state.
 * @ptr: the pointer to convert.
 */
static inline state PTR_ERR(const void *ptr)
{
    return (state)(intptr_t)ptr;
}

/**
 * PTR_INVAL - converts a pointer containing an state to an state or null.
 * @ptr: the pointer to convert.
 */
static inline state PTR_INVAL(const void *ptr)
{
    return PTR_ERR(ptr) ?: -EPERM;
}

/**
 * IS_ERR - Check whether pointer is error value.
 * @ptr: the pointer to check.
 */
static inline bool IS_ERR(const void *ptr)
{
    return (uintptr_t)ptr >= (uintptr_t)-ERRNO_MAX;
}

/**
 * IS_INVAL - Check whether pointer is error value or null.
 * @ptr: the pointer to check.
 */
static inline bool IS_INVAL(const void *ptr)
{
    return unlikely(!ptr) || IS_ERR(ptr);
}

/**
 * PTR_ERR_ZERO - Check whether pointer is error value.
 * @ptr: the pointer to check.
 */
static inline state PTR_ERR_ZERO(const void *ptr)
{
    return unlikely(IS_ERR(ptr)) ? PTR_ERR(ptr) : -ENOERR;
}

/**
 * PTR_INVAL_ZERO - Check whether pointer is error value or null.
 * @ptr: the pointer to check.
 */
static inline state PTR_INVAL_ZERO(const void *ptr)
{
    return unlikely(IS_INVAL(ptr)) ? PTR_INVAL(ptr) : -ENOERR;
}

#endif /* _ERROR_H_ */
