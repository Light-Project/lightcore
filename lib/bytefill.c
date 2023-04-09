/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2022 John Sanpe <sanpeqf@gmail.com>
 */

#include <bytefill.h>
#include <export.h>

BYTEFILL_DECLARE_SUFFIX(, bytefill_short, short, fillfn, (pdata), short (*fillfn)(void *pdata), void *pdata)
BYTEFILL_DECLARE_SUFFIX(, bytefill_int, int, fillfn, (pdata), int (*fillfn)(void *pdata), void *pdata)
BYTEFILL_DECLARE_SUFFIX(, bytefill_long, long, fillfn, (pdata), long (*fillfn)(void *pdata), void *pdata)
BYTEFILL_DECLARE_SUFFIX(, bytefill_longlong, long long, fillfn, (pdata), long long (*fillfn)(void *pdata), void *pdata)

BYTEFILL_DECLARE_SUFFIX(, bytefill_u16, uint16_t, fillfn, (pdata), uint16_t (*fillfn)(void *pdata), void *pdata)
BYTEFILL_DECLARE_SUFFIX(, bytefill_u32, uint32_t, fillfn, (pdata), uint32_t (*fillfn)(void *pdata), void *pdata)
BYTEFILL_DECLARE_SUFFIX(, bytefill_u64, uint64_t, fillfn, (pdata), uint64_t (*fillfn)(void *pdata), void *pdata)

EXPORT_SYMBOL(bytefill_short);
EXPORT_SYMBOL(bytefill_int);
EXPORT_SYMBOL(bytefill_long);
EXPORT_SYMBOL(bytefill_longlong);

EXPORT_SYMBOL(bytefill_u16);
EXPORT_SYMBOL(bytefill_u32);
EXPORT_SYMBOL(bytefill_u64);
