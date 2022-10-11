/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _ASM_GENERIC_BUG_H_
#define _ASM_GENERIC_BUG_H_

#ifndef __ASSEMBLY__
# include <printk.h>
# include <bitflags.h>

enum crash_flags {
    CRASH_FLAG_NCUT_HERE    = 0,
    CRASH_FLAG_WARNING      = 1,
    CRASH_FLAG_ONCE         = 2,
    CRASH_FLAG_DONE         = 3,
};

struct crash_entry {
    uintptr_t addr;
    unsigned long flags;
    const char *file;
    unsigned int line;
};

GENERIC_STRUCT_BITOPS(crash, struct crash_entry, flags);
GENERIC_STRUCT_FLAG(crash, struct crash_entry, flags, ncut_here, CRASH_FLAG_NCUT_HERE);
GENERIC_STRUCT_FLAG(crash, struct crash_entry, flags, warning, CRASH_FLAG_WARNING);
GENERIC_STRUCT_FLAG(crash, struct crash_entry, flags, once, CRASH_FLAG_ONCE);
GENERIC_STRUCT_FLAG(crash, struct crash_entry, flags, done, CRASH_FLAG_DONE);

#ifndef CRASH_CUT_HERE
# define CRASH_CUT_HERE "------------[ CUT HERE ]------------\n"
#endif

#define GENERIC_CRASH_OP(opcode, flags, extra) ({   \
    __label__ __here;                               \
    __here: asm volatile(opcode"\n"extra);          \
    static const struct crash_entry crash __used    \
    __section(".data.bug_table") = {(unsigned long) \
    &&__here, flags, __FILE__, __LINE__};           \
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