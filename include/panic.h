/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _PANIC_H_
#define _PANIC_H_

#include <notifier.h>
#include <crash.h>

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

#endif  /* _PANIC_H_ */
