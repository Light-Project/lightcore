/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _BSEARCH_H_
#define _BSEARCH_H_

#include <types.h>
#include <stddef.h>

typedef long (*bsearch_cmp_t)(const void *key, void *pdata);

static __always_inline void *
bsearch_inline(const void *base, size_t num, size_t esize, bsearch_cmp_t cmp, void *pdata)
{
    const void *pivot;
    long result;

    while (num) {
        pivot = base + (num >> 1) * esize;
        result = cmp(pivot, pdata);

        if (!result)
            return (void *)pivot;

        if (result < 0) {
            base = pivot + esize;
            num--;
        }

        num >>= 1;
    }

    return NULL;
}

extern void *bsearch(const void *base, size_t num, size_t esize, bsearch_cmp_t cmp, void *pdata);

#endif  /* _BSEARCH_H_ */
