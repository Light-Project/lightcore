/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <lib.h>
#include <crc-table.h>

static uint32_t crc32(const uint8_t *src, int len, uint32_t crc)
{
    uint32_t tmp = crc;
    while (len--)
        tmp = (tmp >> 8) ^ crc32_table[(tmp & 0xff) ^ *src++];
    return tmp ^ crc;
}

void kernel_check(void *addr)
{
    struct boot_head *boot_head = addr;
    uint32_t crc32new, crc32old;
    uint32_t size = boot_head->size;

    pr_boot("verification info:\n");
    pr_boot("  start: %p\n", addr);
    pr_boot("  size: %#x\n", size);

    if (strcmp((char *)&boot_head->magic, "lightcore!"))
        panic("can't find kernel!\n");

    crc32old = boot_head->crc;
    crc32new = crc32((uint8_t *)(boot_head + 1), size, 0xffffffff);

    if (crc32old != crc32new)
        panic("crc error 0x%x->0x%x\n", crc32old, crc32new);
}
