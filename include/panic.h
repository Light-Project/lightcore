/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _PANIC_H_
#define _PANIC_H_

#include <list.h>
#include <asm/bug.h>

struct panic_work {
    struct list_head list;
    void (*work)(const void *data);
    const void *data;
};

#define DEBUG_DATA_CHECK(condition, fmt, ...) ({    \
    bool corruption = unlikely(condition);          \
    if (corruption) {                               \
        pr_crit(fmt, ##__VA_ARGS__);                \
        BUG();                                      \
    }                                               \
    corruption;                                     \
})

extern state panic_work_register(struct panic_work *work);
extern __printf(1, 2) __noreturn __cold void panic(const char *fmt, ...);

#endif  /* _PANIC_H_ */
