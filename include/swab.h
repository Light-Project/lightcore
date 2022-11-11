/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _SWAB_H_
#define _SWAB_H_

#include <types.h>
#include <lightcore/swab.h>

static inline void swab16_array(uint16_t *buff, unsigned int words)
{
    while (words--) {
        swab16s(buff);
        buff++;
    }
}

static inline void swab32_array(uint32_t *buff, unsigned int words)
{
    while (words--) {
        swab32s(buff);
        buff++;
    }
}

static inline void swab64_array(uint64_t *buff, unsigned int words)
{
    while (words--) {
        swab64s(buff);
        buff++;
    }
}

#endif  /* _SWAB_H_ */
