/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _CRASH_H_
#define _CRASH_H_

#include <asm/bug.h>

enum crash_type {
    CRASH_TYPE_NONE     = 0,
    CRASH_TYPE_WARN     = 1,
    CRASH_TYPE_BUG      = 2,
};

extern struct crash_entry *crash_find(uintptr_t addr);
extern enum crash_type crash_report(uintptr_t addr, struct regs *regs);
extern void __init crash_init(void);

#endif /* _CRASH_H_ */
