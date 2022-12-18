/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2022 Sanpe <sanpeqf@gmail.com>
 */

#include <crypto/crc.h>
#include <crypto/crc-itut-table.h>
#include <export.h>

uint16_t __weak crc_itut(const uint8_t *src, size_t len, uint16_t crc)
{
    return crc_itut_inline(src, len, crc);
}
EXPORT_SYMBOL(crc_itut);
