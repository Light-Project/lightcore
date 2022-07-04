/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2022 Sanpe <sanpeqf@gmail.com>
 */

#include <crypto/crc.h>
#include <crypto/crc32-table.h>
#include <export.h>

uint32_t crc32(const uint8_t *src, size_t len, uint32_t crc)
{
    return crc32_inline(src, len, crc);
}
EXPORT_SYMBOL(crc32);
