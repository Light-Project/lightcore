/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2022 Sanpe <sanpeqf@gmail.com>
 */

#include <memdump.h>
#include <proc.h>
#include <printk.h>
#include <export.h>

static uint8_t memdump_read8(void *pdata, uintptr_t addr)
{
    return *(uint8_t *)addr;
}

static uint16_t memdump_read16(void *pdata, uintptr_t addr)
{
    return unaligned_get_u16((void *)addr);
}

static uint32_t memdump_read32(void *pdata, uintptr_t addr)
{
    return unaligned_get_u32((void *)addr);
}

static uint64_t memdump_read64(void *pdata, uintptr_t addr)
{
    return unaligned_get_u64((void *)addr);
}

static int memdump_callback(void *pdata, const char *fmt, ...)
{
    va_list args;
    int retval;

    va_start(args, fmt);
    retval = vprintk(fmt, args);
    va_end(args);

    return retval;
}

int memdump_print(fmtdump_print_t func, void *pdata, const char *prefix, enum fmtdump_prefix_mode mode,
                  const void *data, size_t size, int rowsize, int groupsize, int base, bool ascii)
{
    struct fmtdump_ops memdump_ops = {
        .read8 = memdump_read8,
        .read16 = memdump_read16,
        .read32 = memdump_read32,
        .read64 = memdump_read64,
        .print = func,
    };

    return fmtdump_print(
        &memdump_ops, pdata, prefix, mode,
        (uintptr_t)data, size, rowsize, groupsize, base, ascii
    );
}
EXPORT_SYMBOL(memdump_print);

int memdump_printk(const char *prefix, enum fmtdump_prefix_mode mode, const void *data, size_t size,
                   int rowsize, int groupsize, int base, bool ascii)
{
    struct fmtdump_ops memdump_ops = {
        .read8 = memdump_read8,
        .read16 = memdump_read16,
        .read32 = memdump_read32,
        .read64 = memdump_read64,
        .print = memdump_callback,
    };

    return fmtdump_print(
        &memdump_ops, NULL, prefix, mode,
        (uintptr_t)data, size, rowsize, groupsize, base, ascii
    );
}
EXPORT_SYMBOL(memdump_printk);
