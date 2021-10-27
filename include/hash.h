/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _HASH_H_
#define _HASH_H_

#include <types.h>

static __always_inline unsigned long strhash(const char *str)
{
    uint32_t hash = 5381;

    while (*str)
        hash = (hash << 5) + hash ^ *str++;

    return hash;
}

#endif  /* _HASH_H_ */
