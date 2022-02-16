/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _ASM_GENERIC_BUG_H_
#define _ASM_GENERIC_BUG_H_

#ifndef __ASSEMBLY__
#include <printk.h>
#include <panic.h>

#ifndef WARN
# define WARN() do {                                \
    printk("Generic Warn: failure at %s:%d/%s()\n", \
            __FILE__, __LINE__, __func__);          \
} while (0)
#endif

#ifndef BUG
# define BUG() do {                                 \
    printk("Generic BUG: failure at %s:%d/%s()\n",  \
            __FILE__, __LINE__, __func__);          \
    barrier_before_unreachable();                   \
    panic();                                        \
} while (0)
#endif

#ifndef WARN_ON
# define WARN_ON(condition) ({                      \
    if (unlikely(condition))                        \
        WARN();                                     \
    unlikely(condition);                            \
})
#endif

#ifndef BUG_ON
# define BUG_ON(condition) ({                       \
    if (unlikely(condition))                        \
        BUG();                                      \
    unlikely(condition);                            \
})
#endif

#endif  /* __ASSEMBLY__ */
#endif  /* _ASM_GENERIC_BUG_H_ */