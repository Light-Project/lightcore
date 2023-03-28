/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _DATACHECK_H_
#define _DATACHECK_H_

#include <printk.h>
#include <crash.h>

#define DEBUG_DATA_CHECK(condition, fmt, ...) ({    \
    bool corruption = unlikely(condition);          \
    if (corruption) {                               \
        pr_crit(fmt, ##__VA_ARGS__);                \
        if (IS_ENABLED(CONFIG_DATA_CHECK_BUG))      \
            BUG();                                  \
        else                                        \
            WARN();                                 \
    }                                               \
    corruption;                                     \
})

#endif /* _DATACHECK_H_ */
