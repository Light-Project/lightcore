/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _ASM_GENERIC_BUG_H_
#define _ASM_GENERIC_BUG_H_

#ifndef __ASSEMBLY__
# include <printk.h>
# include <panic.h>

#define GENERIC_CLASH_OP(opcode, type, extra) ({    \
    __label__ __here;                               \
    __here: asm volatile(BUG_OPCODE"\n"extra);      \
    static const struct crash_entry crash __used    \
    __section(".data.bug_table") = {(unsigned long) \
    &&__here, type, __FILE__, __func__, __LINE__};  \
})

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
    panic("Generic BUG");                           \
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