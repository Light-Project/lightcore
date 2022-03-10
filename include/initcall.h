/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _INITCALL_H_
#define _INITCALL_H_

#include <types.h>
#include <state.h>

typedef state (*initcall_t)(void);

struct initcall_entry {
    const char *name;
    initcall_t fn;
};

#ifdef CONFIG_ARCH_HAS_PREL32_RELOCATIONS
typedef int initcall_entry_t;
static inline initcall_t initcall_from_entry(initcall_entry_t *entry)
{
    return offset_to_ptr(entry);
}
#else
typedef struct initcall_entry initcall_entry_t;
#define initcall_from_entry(entry)  (entry->fn)
#endif

extern initcall_entry_t _ld_initcall_start[];
extern initcall_entry_t _ld_initcall0_start[];
extern initcall_entry_t _ld_initcall1_start[];
extern initcall_entry_t _ld_initcall2_start[];
extern initcall_entry_t _ld_initcall3_start[];
extern initcall_entry_t _ld_initcall4_start[];
extern initcall_entry_t _ld_initcall5_start[];
extern initcall_entry_t _ld_initcall6_start[];
extern initcall_entry_t _ld_initcall7_start[];
extern initcall_entry_t _ld_initcall8_start[];
extern initcall_entry_t _ld_initcall_end[];

extern initcall_entry_t _ld_clk_initcall_start[];
extern initcall_entry_t _ld_clk_initcall_end[];
extern initcall_entry_t _ld_irqchip_initcall_start[];
extern initcall_entry_t _ld_irqchip_initcall_end[];
extern initcall_entry_t _ld_clockevent_initcall_start[];
extern initcall_entry_t _ld_clockevent_initcall_end[];
extern initcall_entry_t _ld_clocksource_initcall_start[];
extern initcall_entry_t _ld_clocksource_initcall_end[];
extern initcall_entry_t _ld_pconsole_initcall_start[];
extern initcall_entry_t _ld_pconsole_initcall_end[];
extern initcall_entry_t _ld_console_initcall_start[];
extern initcall_entry_t _ld_console_initcall_end[];
extern initcall_entry_t _ld_scheduler_initcall_start[];
extern initcall_entry_t _ld_scheduler_initcall_end[];
extern initcall_entry_t _ld_kshell_initcall_start[];
extern initcall_entry_t _ld_kshell_initcall_end[];

/* Format: <modname>__<counter>_<line>_<fn> */
#define __initcall_id(fn)                               \
        __PASTE(__KBUILD_MODNAME,                       \
        __PASTE(__,                                     \
        __PASTE(__COUNTER__,                            \
        __PASTE(_,                                      \
        __PASTE(__LINE__,                               \
        __PASTE(_, fn))))))

/* Format: __<prefix>__<iid><id> */
#define __initcall_name(prefix, __iid, id)              \
        __PASTE(__,                                     \
        __PASTE(prefix,                                 \
        __PASTE(__,                                     \
        __PASTE(__iid, id))))

#define __define_initcall_stub(__stub, fn)              \
        __ADDRESSABLE(fn)

#define __initcall_stub(fn, __iid, id) fn
#define __initcall_section(__sec, __iid) ".init" #__sec

#define ____define_section(fn, __stub, __name, __sec)   \
        __define_initcall_stub(__stub, fn)              \
        static const struct initcall_entry __name       \
        __used __section(__sec) = { #fn , __stub}

#define __unique_initcall(fn, id, __sec, __iid)         \
    ____define_section(fn,                              \
        __initcall_stub(fn, __iid, id),                 \
        __initcall_name(initcall, __iid, id),           \
        __initcall_section(__sec, __iid))

#define ___define_initcall(fn, id, __sec) \
    __unique_initcall(fn, id, __sec, __initcall_id(fn))

#define __define_initcall(fn, id) ___define_initcall(fn, id, .initcall_##id)

#define core_initcall(fn)               __define_initcall(fn, 0)
#define core_initcall_sync(fn)          __define_initcall(fn, 0s)
#define arch_initcall(fn)               __define_initcall(fn, 1)
#define arch_initcall_sync(fn)          __define_initcall(fn, 1s)
#define framework_initcall(fn)          __define_initcall(fn, 2)
#define framework_initcall_sync(fn)     __define_initcall(fn, 2s)
#define fs_initcall(fn)                 __define_initcall(fn, 3)
#define fs_initcall_sync(fn)            __define_initcall(fn, 3s)
#define driver_initcall(fn)             __define_initcall(fn, 7)
#define driver_initcall_sync(fn)        __define_initcall(fn, 7s)
#define late_initcall(fn)               __define_initcall(fn, 8)
#define late_initcall_sync(fn)          __define_initcall(fn, 8s)

#define clk_initcall(fn)                ___define_initcall(fn, clk, .clk_initcall)
#define irqchip_initcall(fn)            ___define_initcall(fn, irq, .irqchip_initcall)
#define clockevent_initcall(fn)         ___define_initcall(fn, tim, .clockevent_initcall)
#define clocksource_initcall(fn)        ___define_initcall(fn, tim, .clocksource_initcall)
#define pconsole_initcall(fn)           ___define_initcall(fn, pco, .pconsole_initcall)
#define console_initcall(fn)            ___define_initcall(fn, con, .console_initcall)
#define scheduler_initcall(fn)          ___define_initcall(fn, sch, .scheduler_initcall)
#define kshell_initcall(fn)             ___define_initcall(fn, ksh, .kshell_initcall)

#define initcall_for_each_fn(fn, sec)                   \
    for (fn = (initcall_entry_t *)&_ld_##sec##_start;   \
         fn < (initcall_entry_t *)&_ld_##sec##_end; ++fn)

struct bootarg {
    const char *args;
    state (*fn)(const char *);
};

extern struct bootarg _ld_boot_param_start[];
extern struct bootarg _ld_boot_param_end[];

#define define_bootarg(str, unique_id, fn)              \
        static struct bootarg __boot_##unique_id        \
        __used __section(".init.bootargs")              \
        __aligned(__alignof__(struct bootarg)) = {str,fn}

#define bootarg_initcall(str, fn) define_bootarg(str, fn, fn)

#define initcall_for_each_args(args, sec)               \
    for(args = (struct bootarg *)&_ld_##sec##_start;    \
        args < (struct bootarg *)&_ld_##sec##_end; ++args)

extern void initcalls(void);
extern void bootargs_setup(void);

#endif  /* _INITCALL_H_ */
