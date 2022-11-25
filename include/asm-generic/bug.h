/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _ASM_GENERIC_BUG_H_
#define _ASM_GENERIC_BUG_H_

#ifndef __ASSEMBLY__
# include <printk.h>
# include <bitflags.h>
# include <once.h>

enum crash_flags {
    __CRASH_FLAG_NCUT_HERE  = 0,
    __CRASH_FLAG_WARNING    = 1,
    __CRASH_FLAG_ONCE       = 2,
    __CRASH_FLAG_DONE       = 3,
};

#define CRASH_FLAG_NCUT_HERE    BIT(__CRASH_FLAG_NCUT_HERE)
#define CRASH_FLAG_WARNING      BIT(__CRASH_FLAG_WARNING)
#define CRASH_FLAG_ONCE         BIT(__CRASH_FLAG_ONCE)
#define CRASH_FLAG_DONE         BIT(__CRASH_FLAG_DONE)

struct crash_entry {
    uintptr_t addr;
    const char *file;
    unsigned long flags;
    unsigned int line;
};

GENERIC_STRUCT_BITOPS(crash, struct crash_entry, flags, false);
GENERIC_STRUCT_FLAG(crash, struct crash_entry, flags, ncut_here, __CRASH_FLAG_NCUT_HERE);
GENERIC_STRUCT_FLAG(crash, struct crash_entry, flags, warning, __CRASH_FLAG_WARNING);
GENERIC_STRUCT_FLAG(crash, struct crash_entry, flags, once, __CRASH_FLAG_ONCE);
GENERIC_STRUCT_FLAG(crash, struct crash_entry, flags, done, __CRASH_FLAG_DONE);

#ifndef CRASH_CUT_HERE
# define CRASH_CUT_HERE "------------[ CUT HERE ]------------\n"
#endif

#define GENERIC_CRASH_OP(opcode, flags, extra) ({   \
    asm volatile (                                  \
        ".pushsection .data.bug_table, \"aw\"   \n" \
        "1: .long   2f                          \n" \
        "   .long   %c0                         \n" \
        "   .long   %c2                         \n" \
        "   .int    %c1                         \n" \
		"   .org    1b + %c3                    \n" \
		".popsection                            \n" \
        "2: "opcode"\n"extra"                   \n" \
        : : "i" (__FILE__), "i" (__LINE__),         \
        "i" (flags),                                \
        "i" (sizeof(struct crash_entry))            \
    );                                              \
})

#ifdef CRASH_FLAGS

#ifndef BUG
# define BUG() ({ \
    CRASH_FLAGS(0); \
    unreachable(); \
})
#endif

#ifndef WARN
# define WARN() \
    CRASH_FLAGS(CRASH_FLAG_WARNING)
#endif

#ifndef WARN_ONCE
# define WARN_ONCE() \
    CRASH_FLAGS(CRASH_FLAG_WARNING | CRASH_FLAG_ONCE)
#endif

#endif /* CRASH_FLAGS */

#ifndef BUG
# define BUG() do {                                 \
    printk("Generic BUG: failure at %s:%d\n",       \
            __FILE__, __LINE__);                    \
    barrier_before_unreachable();                   \
    panic("Generic BUG");                           \
} while (0)
#endif

#ifndef WARN
# define WARN() do {                                \
    printk("Generic Warn: failure at %s:%d\n",      \
            __FILE__, __LINE__);                    \
} while (0)
#endif

#ifndef BUG_ON
# define BUG_ON(condition) ({                       \
    bool __cond = !!(condition);                    \
    if (unlikely(__cond))                           \
        BUG();                                      \
    unlikely(__cond);                               \
})
#endif

#ifndef WARN_ON
# define WARN_ON(condition) ({                      \
    bool __cond = !!(condition);                    \
    if (unlikely(__cond))                           \
        WARN();                                     \
    unlikely(__cond);                               \
})
#endif

#ifndef WARN_ONCE
# define WARN_ONCE() DO_ONCE(WARN)
#endif

#ifndef WARN_ON_ONCE
# define WARN_ON_ONCE(condition) ({                 \
    bool __cond = !!(condition);                    \
    if (unlikely(__cond))                           \
        WARN_ONCE();                                \
    unlikely(__cond);                               \
})
#endif

#endif  /* __ASSEMBLY__ */
#endif  /* _ASM_GENERIC_BUG_H_ */