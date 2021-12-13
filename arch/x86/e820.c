/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#define pr_fmt(fmt) "bios-e820: " fmt

#include <printk.h>
#include <mm/memblock.h>
#include <asm/e820.h>

static const char *e820_to_name(enum e820_type type)
{
    switch (type) {
        case E820_TYPE_RAM: return "e820-usable";
        case E820_TYPE_RESERVED: return "e820-reserved";
        case E820_TYPE_ACPI: return "e820-acpi_data";
        case E820_TYPE_NVS: return "e820-acpi_nvs";
        case E820_TYPE_UNUSABLE: return "e820-unusable";
        case E820_TYPE_PMEM: return "e820-persistent_legacy";
        case E820_TYPE_PRAM: return "e820-persistent";
        default: return "e820-unknow";
    }
}

void e820_init(struct e820_table *e820_table)
{
    struct e820_entry *entry;
    unsigned int count;
    const char *name;
    uint64_t end;

    pr_info("e820 layout:\n");

    for (count = 0; count < e820_table->entry_nr; ++count) {
        entry = &e820_table->entry[count];
        end = entry->addr + entry->size - 1;
        name = e820_to_name(entry->type);

        pr_info("  [%#018llx-%#018llx] %s (%u)\n",
                entry->addr, end, name, entry->type);

        if (end != (resource_size_t)end)
            continue;

        if (entry->type == E820_TYPE_RAM)
            memblock_add(name, entry->addr, entry->size);
        else
            memblock_reserve(name, entry->addr, entry->size);
    }
}
