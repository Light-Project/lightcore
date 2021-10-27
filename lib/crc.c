/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <crc.h>
#include <crc_table.h>
#include <export.h>

uint8_t crc4(uint8_t *src, int len, uint8_t crc)
{
    uint8_t tmp = crc;

    len *= 2;
    while (len--)
        tmp = crc4_table[(tmp & 0x0f) ^ *src++];

    return tmp ^ crc;
}

uint8_t crc8(uint8_t *src, int len, uint8_t crc)
{
    uint8_t tmp = crc;

    while (len--)
        tmp = crc8_table[(tmp & 0xff) ^ *src++];

    return tmp ^ crc;
}

uint16_t crc16(uint8_t *src, int len, uint16_t crc)
{
    uint32_t tmp = crc;

    while (len--)
        tmp = (tmp << 8) ^ crc16_table[(tmp & 0xff) ^ *src++];

    return tmp ^ crc;
}

uint32_t crc32(const uint8_t *src, int len, uint32_t crc)
{
    uint32_t tmp = crc;

    while (len--)
        tmp = (tmp >> 8) ^ crc32_table[(tmp & 0xff) ^ *src++];

    return tmp ^ crc;
}

EXPORT_SYMBOL(crc4);
EXPORT_SYMBOL(crc8);
EXPORT_SYMBOL(crc16);
EXPORT_SYMBOL(crc32);
