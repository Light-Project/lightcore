/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _LIGHTCORE_KERNEL_H_
#define _LIGHTCORE_KERNEL_H_

#include <lightcore/types.h>
#include <lightcore/errno.h>

/**
 * ARRAY_SIZE - get the number of elements in array.
 * @arr: array to be sized.
 */
#define ARRAY_SIZE(arr) ( \
    sizeof(arr) / sizeof((arr)[0]) \
)

#endif /* _LIGHTCORE_KERNEL_H_ */
