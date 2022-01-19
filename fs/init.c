/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <filesystem.h>
#include <kmalloc.h>
#include <export.h>

struct kcache *name_cache;
EXPORT_SYMBOL(name_cache);

void __init vfl_init(void)
{
    // name_cache = kcache_create(, , KCACHE_PANIC);

}
