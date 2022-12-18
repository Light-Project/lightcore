/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2022 Sanpe <sanpeqf@gmail.com>
 */

#include <bits.h>
#include <minmax.h>

#include <crypto/crc.h>
#include <crypto/crc4-table.h>
#include <export.h>

uint8_t __weak crc4(const uint8_t *src, size_t len, uint8_t crc)
{
    return crc4_inline(src, len, crc);
}
EXPORT_SYMBOL(crc4);
