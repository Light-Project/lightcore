/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _HASH_H_
#define _HASH_H_

static unsigned long strhash(const char *str)
{
    uint32_t hash = 5320;
    if(str == NULL)
        return 0;
    while(*str != '\0')
        hash = (hash << 2) + hash + (*str++);
    return hash;
}

#endif /*  */
