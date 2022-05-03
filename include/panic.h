/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _PANIC_H_
#define _PANIC_H_

#include <list.h>
#include <asm/bug.h>
#include <notifier.h>

enum crash_type {
    CRASH_TYPE_NONE,
    CRASH_TYPE_WARN,
    CRASH_TYPE_BUG,
};

struct crash_entry {
    uintptr_t addr;
    enum crash_type type;
    const char *file;
    const char *func;
    unsigned int line;
};

#define DEBUG_DATA_CHECK(condition, fmt, ...) ({    \
    bool corruption = unlikely(condition);          \
    if (corruption) {                               \
        pr_crit(fmt, ##__VA_ARGS__);                \
        BUG();                                      \
    }                                               \
    corruption;                                     \
})

DECLARE_NOTIFIER_SPIN_HEAD(panic_notifier);
extern __printf(1, 2) __noreturn __cold void panic(const char *fmt, ...);
extern enum crash_type crash_report(uintptr_t addr, struct regs *regs);

#endif  /* _PANIC_H_ */
