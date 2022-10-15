/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2022 Sanpe <sanpeqf@gmail.com>
 */

#include <sort.h>
#include <string.h>
#include <export.h>

static __always_inline void
sort_swap(size_t cells, void *cela, void *celb)
{
    uint8_t buff[cells];

    memcpy(buff, cela, cells);
    memcpy(cela, celb, cells);
    memcpy(celb, buff, cells);
}

static __attribute_const__ __always_inline size_t
parent(size_t cells, unsigned int lsbit, size_t index)
{
    index -= cells;
    index -= cells & -(index & lsbit);
    return index >> 1;
}

state qsort(void *base, size_t number, size_t cells, sort_cmp_t cmp, void *pdata)
{
    const unsigned int lsbit = cells & -cells;
    size_t size = number * cells;
    size_t ida, idb, idc, idd;

    ida = (number / 2) * cells;
    if (!base || !cmp || !ida)
        return -EINVAL;

    for (;;) {
        if (ida)
            ida -= cells;
        else if (size -= cells)
            sort_swap(cells, base, base + size);
        else
            break;

        for (idb = ida; idc = 2 * idb + cells, (idd = idc + cells) < size;)
            idb = cmp(base + idc, base + idd, pdata) >= 0 ? idc : idd;
        if (idd == size)
            idb = idc;

        while (ida != idb && cmp(base + ida, base + idb, pdata) >= 0)
            idb = parent(cells, lsbit, idb);

        for (idc = idb; ida != idb;) {
            idb = parent(cells, lsbit, idb);
            sort_swap(cells, base + idb, base + idc);
        }
    }

    return -ENOERR;
}
EXPORT_SYMBOL(qsort);

state bsort(void *base, size_t number, size_t cells, sort_cmp_t cmp, void *pdata)
{
    void *walk, *prev;
    unsigned int count;
    bool swapped;

    if (!base || number < 2 || !cells || !cmp)
        return -EINVAL;

    for (swapped = true; swapped &&
         ({swapped = false; true;}); --number) {
        prev = base;
        for (count = 1; count < number; ++count) {
            walk = base + cells * count;
            if (cmp(prev, walk, pdata) > 0) {
                sort_swap(cells, walk, prev);
                swapped = true;
            }
            prev = walk;
        }
    }

    return -ENOERR;
}
EXPORT_SYMBOL(bsort);
