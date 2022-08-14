/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2022 Sanpe <sanpeqf@gmail.com>
 */

#include <crypto/crc.h>
#include <crypto/crc64-table.h>
#include <export.h>

uint64_t crc64(const uint8_t *src, size_t len, uint64_t crc)
{
    return crc64_inline(src, len, crc);
}
EXPORT_SYMBOL(crc64);
