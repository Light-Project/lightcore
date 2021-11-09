/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <size.h>
#include <initcall.h>
#include <driver/platform.h>
#include <driver/mtd/spinor.h>

static const struct spinor_type winbond_types[] = {
    { .name = "w25x05",         .jedec = 0xef3010, .page_size = 256, .sector_size = 64 * SZ_1KiB, .sector_nr =    1 },
    { .name = "w25x10",         .jedec = 0xef3011, .page_size = 256, .sector_size = 64 * SZ_1KiB, .sector_nr =    2 },
    { .name = "w25x20",         .jedec = 0xef3012, .page_size = 256, .sector_size = 64 * SZ_1KiB, .sector_nr =    4 },
    { .name = "w25x40",         .jedec = 0xef3013, .page_size = 256, .sector_size = 64 * SZ_1KiB, .sector_nr =    8 },
    { .name = "w25x80",         .jedec = 0xef3014, .page_size = 256, .sector_size = 64 * SZ_1KiB, .sector_nr =   16 },
    { .name = "w25x16",         .jedec = 0xef3015, .page_size = 256, .sector_size = 64 * SZ_1KiB, .sector_nr =   32 },
    { .name = "w25x32",         .jedec = 0xef3016, .page_size = 256, .sector_size = 64 * SZ_1KiB, .sector_nr =   32 },
    { .name = "w25x64",         .jedec = 0xef3017, .page_size = 256, .sector_size = 64 * SZ_1KiB, .sector_nr =  128 },
    { .name = "w25q20cl",       .jedec = 0xef4012, .page_size = 256, .sector_size = 64 * SZ_1KiB, .sector_nr =    4 },
    { .name = "w25q80bl",       .jedec = 0xef4014, .page_size = 256, .sector_size = 64 * SZ_1KiB, .sector_nr =   16 },
    { .name = "w25q32",         .jedec = 0xef4016, .page_size = 256, .sector_size = 64 * SZ_1KiB, .sector_nr =   64 },
    { .name = "w25q64",         .jedec = 0xef4017, .page_size = 256, .sector_size = 64 * SZ_1KiB, .sector_nr =  128 },
    { .name = "w25q128",        .jedec = 0xef4018, .page_size = 256, .sector_size = 64 * SZ_1KiB, .sector_nr =  256 },
    { .name = "w25q256",        .jedec = 0xef4019, .page_size = 256, .sector_size = 64 * SZ_1KiB, .sector_nr =  512 },
    { .name = "w25q512jvq",     .jedec = 0xef4020, .page_size = 256, .sector_size = 64 * SZ_1KiB, .sector_nr = 1024 },
    { .name = "w25q20bw",       .jedec = 0xef5012, .page_size = 256, .sector_size = 64 * SZ_1KiB, .sector_nr =    4 },
    { .name = "w25q80",         .jedec = 0xef5014, .page_size = 256, .sector_size = 64 * SZ_1KiB, .sector_nr =   16 },
    { .name = "w25q20ew",       .jedec = 0xef6012, .page_size = 256, .sector_size = 64 * SZ_1KiB, .sector_nr =    4 },
    { .name = "w25q16dw",       .jedec = 0xef6015, .page_size = 256, .sector_size = 64 * SZ_1KiB, .sector_nr =   64 },
    { .name = "w25q32dw",       .jedec = 0xef6016, .page_size = 256, .sector_size = 64 * SZ_1KiB, .sector_nr =   64 },
    { .name = "w25q64dw",       .jedec = 0xef6017, .page_size = 256, .sector_size = 64 * SZ_1KiB, .sector_nr =  128 },
    { .name = "w25q128fw",      .jedec = 0xef6018, .page_size = 256, .sector_size = 64 * SZ_1KiB, .sector_nr =  256 },
    { .name = "w25q256jw",      .jedec = 0xef6019, .page_size = 256, .sector_size = 64 * SZ_1KiB, .sector_nr =  512 },
    { .name = "w25q16jv-im/jm", .jedec = 0xef7015, .page_size = 256, .sector_size = 64 * SZ_1KiB, .sector_nr =    4 },
    { .name = "w25q32jv",       .jedec = 0xef7016, .page_size = 256, .sector_size = 64 * SZ_1KiB, .sector_nr =   64 },
    { .name = "w25q64jvm",      .jedec = 0xef7017, .page_size = 256, .sector_size = 64 * SZ_1KiB, .sector_nr =  128 },
    { .name = "w25q128jv",      .jedec = 0xef7018, .page_size = 256, .sector_size = 64 * SZ_1KiB, .sector_nr =  256 },
    { .name = "w25q256jvm",     .jedec = 0xef7019, .page_size = 256, .sector_size = 64 * SZ_1KiB, .sector_nr =  512 },
    { .name = "w25m512jv",      .jedec = 0xef7119, .page_size = 256, .sector_size = 64 * SZ_1KiB, .sector_nr = 1024 },
    { .name = "w25q32jwm",      .jedec = 0xef8016, .page_size = 256, .sector_size = 64 * SZ_1KiB, .sector_nr =   64 },
    { .name = "w25q64jwm",      .jedec = 0xef8017, .page_size = 256, .sector_size = 64 * SZ_1KiB, .sector_nr =  128 },
    { .name = "w25q128jwm",     .jedec = 0xef8018, .page_size = 256, .sector_size = 64 * SZ_1KiB, .sector_nr =  256 },
    { .name = "w25q256jwm",     .jedec = 0xef8019, .page_size = 256, .sector_size = 64 * SZ_1KiB, .sector_nr =  512 },
    { },
};

static state winbond_init(void)
{
    spinor_type_register(winbond_types);
}
driver_initcall(winbond_init);
