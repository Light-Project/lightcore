/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 John Sanpe <sanpeqf@gmail.com>
 */

#include <lib.h>
#include <crypto/crc32-table.h>

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
    crc32new = crc32_inline((uint8_t *)(boot_head + 1), size, 0xffffffff);

    if (crc32old != crc32new)
        panic("crc error 0x%x->0x%x\n", crc32old, crc32new);
}
