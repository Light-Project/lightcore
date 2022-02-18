/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <crypto.h>
#include <crc-table.h>
#include <export.h>

uint8_t crc4(uint8_t *src, int len, uint8_t crc)
{
    return crc4_inline(src, len, crc);
}
EXPORT_SYMBOL(crc4);

uint8_t crc8(uint8_t *src, int len, uint8_t crc)
{
    return crc8_inline(src, len, crc);
}
EXPORT_SYMBOL(crc8);

uint16_t crc16(uint8_t *src, int len, uint16_t crc)
{
    return crc16_inline(src, len, crc);
}
EXPORT_SYMBOL(crc16);

uint32_t crc32(const uint8_t *src, int len, uint32_t crc)
{
    return crc32_inline(src, len, crc);
}
EXPORT_SYMBOL(crc32);
