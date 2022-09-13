/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2022 Sanpe <sanpeqf@gmail.com>
 */

#include <bcd.h>
#include <export.h>

uint8_t bcd2bin_dynamic(uint8_t bcd)
{
    return bcd2bin_const(bcd);
}
EXPORT_SYMBOL(bcd2bin_dynamic);

uint8_t bin2bcd_dynamic(uint8_t bin)
{
    return bin2bcd_const(bin);
}
EXPORT_SYMBOL(bin2bcd_dynamic);
