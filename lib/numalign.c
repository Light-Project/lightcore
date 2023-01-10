/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2023 Sanpe <sanpeqf@gmail.com>
 */

#include <numalign.h>
#include <kernel.h>
#include <log2.h>
#include <export.h>

/* Upper limit of octal length */
static unsigned int oct_align_table[] = {
    /* 0, 377 */ 3,
    /* 0, 177777 */ 5,
    /* 0, 37777777777 */ 11,
    /* 0, 1777777777777777777777 */ 22,
};

/* Upper limit of signed decimal length */
static unsigned int dec_signed_align_table[] = {
    /* -128, 127 */ 4,
    /* -32768, 32767 */ 6,
    /* -2147483648, 2147483647 */ 11,
    /* -9223372036854775808, 9223372036854775807 */ 20,
};

/* Upper limit of unsigned decimal length */
static unsigned int dec_unsigned_align_table[] = {
    /* 0, 255 */ 3,
    /* 0, 65535 */ 5,
    /* 0, 4294967295 */ 10,
    /* 0, 18446744073709551615 */ 20,
};

static unsigned int
common_align(unsigned int *table, size_t limit, unsigned int byte)
{
    unsigned int index;

    if (!is_power_of_2(byte))
        return 0;

    index = ilog2(byte);
    if (index >= limit)
        return 0;

    return table[index];
}

unsigned int oct_align(unsigned int byte)
{
    return common_align(oct_align_table,
        ARRAY_SIZE(oct_align_table), byte
    );
}
EXPORT_SYMBOL(oct_align);

unsigned int dec_signed_align(unsigned int byte)
{
    return common_align(dec_signed_align_table,
        ARRAY_SIZE(dec_signed_align_table), byte
    );
}
EXPORT_SYMBOL(dec_signed_align);

unsigned int dec_unsigned_align(unsigned int byte)
{
    return common_align(dec_unsigned_align_table,
        ARRAY_SIZE(dec_unsigned_align_table), byte
    );
}
EXPORT_SYMBOL(dec_unsigned_align);
