/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _ONCE_H_
#define _ONCE_H_

#include <types.h>
#include <stddef.h>

#define DO_ONCE_DONE(condition) ({                  \
    static bool __section(".data.once") __already;  \
    bool __cond = !!(condition);                    \
    bool __retval = false;                          \
                                                    \
    if (unlikely(__cond && !__already)) {           \
        __already = true;                           \
        __retval = true;                            \
    }                                               \
                                                    \
    unlikely(__retval);                             \
})

#define DO_ONCE_ON(condition, func, ...) ({         \
    bool __cond = !!(condition);                    \
    if (DO_ONCE_DONE(__cond))                       \
        func(__VA_ARGS__);                          \
    unlikely(__cond);                               \
})

#define DO_ONCE(func, ...) \
    DO_ONCE_ON(true, func, ##__VA_ARGS__)

#endif /* _ONCE_H_ */
