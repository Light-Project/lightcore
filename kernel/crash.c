/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#define MODULE_NAME "crash"
#define pr_fmt(fmt) MODULE_NAME ": " fmt

#include <crash.h>
#include <sort.h>
#include <bsearch.h>
#include <sections.h>
#include <printk.h>

static long crash_table_search(const void *key, void *pdata)
{
    const struct crash_entry *crash = key;
    uintptr_t addr = (uintptr_t)pdata;
    if (crash->addr == addr) return 0;
    return crash->addr > addr ? 1 : -1;
}

static long crash_table_sort(const void *nodea, const void *nodeb, void *pdata)
{
    const struct crash_entry *crasha = nodea;
    const struct crash_entry *crashb = nodeb;
    if (crasha->addr == crashb->addr) return 0;
    return crasha->addr > crashb->addr ? 1 : -1;
}

struct crash_entry *crash_find(uintptr_t addr)
{
    struct crash_entry *walk;
    walk = bsearch(_ld_data_bug_table_start, ((uintptr_t)_ld_data_bug_table_end -
                   (uintptr_t)_ld_data_bug_table_start) / sizeof(struct crash_entry),
                   sizeof(struct crash_entry), crash_table_search, (void *)addr);
    return walk;
}

void __init crash_init(void)
{
    qsort(_ld_data_bug_table_start, ((uintptr_t)_ld_data_bug_table_end -
          (uintptr_t)_ld_data_bug_table_start) / sizeof(struct crash_entry),
          sizeof(struct crash_entry), crash_table_sort, NULL);
}

enum crash_type crash_report(uintptr_t addr, struct regs *regs)
{
    struct crash_entry *crash;
    enum crash_type type;
    bool warn;

    crash = crash_find(addr);
    if (unlikely(!crash))
        return CRASH_TYPE_NONE;

    warn = crash_test_warning(crash);
    type = warn ? CRASH_TYPE_WARN : CRASH_TYPE_BUG;

    if (crash_test_once(crash)) {
        if (crash_test_set_done(crash))
            return type;
    }

    if (!crash_test_ncut_here(crash))
        printk(CRASH_CUT_HERE);

    pr_crit("Kernel BUG at %s:%u!\n", crash->file, crash->line);
    return type;
}
