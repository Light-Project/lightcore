/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2022 Sanpe <sanpeqf@gmail.com>
 */

#include <crypto/crc.h>
#include <crypto/crc-rocksoft-table.h>
#include <export.h>

uint64_t __weak crc_rocksoft(const uint8_t *src, size_t len, uint64_t crc)
{
    return crc_rocksoft_inline(src, len, crc);
}
EXPORT_SYMBOL(crc_rocksoft);
