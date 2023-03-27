/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _ONCE_H_
#define _ONCE_H_

#include <types.h>
#include <stddef.h>

#define DO_ONCE_DONE(condition) ({                  \
    static bool __once __done_already;              \
    bool __done_cond = !!(condition);               \
    bool __done_retval = false;                     \
                                                    \
    if (unlikely(__done_cond && !__done_already)) { \
        __done_already = true;                      \
        __done_retval = true;                       \
    }                                               \
                                                    \
    unlikely(__done_retval);                        \
})

#define DO_ONCE_ON(condition, func, ...) ({         \
    bool __once_cond = !!(condition);               \
    if (DO_ONCE_DONE(__once_cond))                  \
        func(__VA_ARGS__);                          \
    unlikely(__once_cond);                          \
})

#define DO_ONCE(func, ...) \
    DO_ONCE_ON(true, func, ##__VA_ARGS__)

#endif /* _ONCE_H_ */
