/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2022 Sanpe <sanpeqf@gmail.com>
 */

#include <kasan.h>
#include <kasan-check.h>
#include <export.h>

bool kasan_check_read(const volatile void *addr, unsigned int size)
{
    return kasan_check_region((uintptr_t)addr, size, false, _RET_IP_);
}
EXPORT_SYMBOL(kasan_check_read);

bool kasan_check_write(const volatile void *addr, unsigned int size)
{
    return kasan_check_region((uintptr_t)addr, size, true, _RET_IP_);
}
EXPORT_SYMBOL(kasan_check_write);
