/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2022 Sanpe <sanpeqf@gmail.com>
 */

#include <extable.h>
#include <sort.h>
#include <bsearch.h>
#include <sections.h>

static long extable_search(const void *key, void *pdata)
{
    const struct extable_entry *extable = key;
    uintptr_t addr = (uintptr_t)pdata;
    if (extable->addr == addr) return 0;
    return extable->addr > addr ? 1 : -1;
}

static long extable_sort(const void *nodea, const void *nodeb, void *pdata)
{
    const struct extable_entry *extablea = nodea;
    const struct extable_entry *extableb = nodeb;
    if (extablea->addr == extableb->addr) return 0;
    return extablea->addr > extableb->addr ? 1 : -1;
}

struct extable_entry *extable_find(uintptr_t addr)
{
    struct extable_entry *walk;
    walk = bsearch(_ld_data_extable_start, ((uintptr_t)_ld_data_extable_end -
                   (uintptr_t)_ld_data_extable_start) / sizeof(struct extable_entry),
                   sizeof(struct extable_entry), extable_search, (void *)addr);
    return walk;
}

void __init extable_init(void)
{
    qsort(_ld_data_extable_start, ((uintptr_t)_ld_data_extable_end -
          (uintptr_t)_ld_data_extable_start) / sizeof(struct extable_entry),
          sizeof(struct extable_entry), extable_sort, NULL);
}
