#ifndef _INITCALL_H_
#define _INITCALL_H_

#include <types.h>
#include <state.h>

typedef state (*initcall_t)(void);

#ifdef CONFIG_ARCH_HAS_PREL32_RELOCATIONS
typedef int initcall_entry_t;

static inline initcall_t initcall_from_entry(initcall_entry_t *entry)
{
    return offset_to_ptr(entry);
}
#else
typedef initcall_t initcall_entry_t;
#define initcall_from_entry(entry)  (*entry)
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
extern initcall_entry_t _ld_initcall_end[];

extern initcall_entry_t _ld_console_initcall_start[];
extern initcall_entry_t _ld_console_initcall_end[];

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
    
#define __initcall_stub(fn, __iid, id)  fn

#define __initcall_section(__sec, __iid)                \
        #__sec ".init"

#if 0
#define ____define_section(fn, __stub, __name, __sec)   \
        __define_initcall_stub(__stub, fn)              \
        asm(".section   \"" __sec "\", \"a\"        \n" \
            __stringify(__name) ":                  \n" \
            ".long  " __stringify(__stub) " - .     \n" \
            ".previous                              \n");
#else
#define ____define_section(fn, __stub, __name, __sec)   \
        __define_initcall_stub(__stub, fn)              \
        static const initcall_t __name __used           \
        __attribute__((__section__(__sec))) = __stub;
#endif

#define __unique_initcall(fn, id, __sec, __iid)         \
        ____define_section(fn,                          \
            __initcall_stub(fn, __iid, id),             \
            __initcall_name(initcall, __iid, id),       \
            __initcall_section(__sec, __iid))

#define ___define_initcall(fn, id, __sec)               \
        __unique_initcall(fn, id, __sec, __initcall_id(fn))
        
#define __define_initcall(fn, id) ___define_initcall(fn, id, .initcall_##id)
    
/*
 * Early initcalls run before initializing SMP.
 *
 * Only for built-in code, not modules.
 */
#define early_initcall(fn)              __define_initcall(fn, early)
/* Only used to initialize dynamic variables */
#define pure_initcall(fn)               __define_initcall(fn, 0)
/* Initialization of important core components */
#define core_initcall(fn)               __define_initcall(fn, 1)
#define core_initcall_sync(fn)          __define_initcall(fn, 1s)
/* initialize arch specific content */
#define arch_initcall(fn)               __define_initcall(fn, 2)
#define arch_initcall_sync(fn)          __define_initcall(fn, 2s)
/* initialize arch specific content */
#define framework_initcall(fn)          __define_initcall(fn, 3)
#define framework_initcall_sync(fn)     __define_initcall(fn, 3s)
#define fs_initcall(fn)                 __define_initcall(fn, 4)
#define fs_initcall_sync(fn)            __define_initcall(fn, 4s)
#define rootfs_initcall(fn)             __define_initcall(fn, 5)
#define driver_initcall(fn)             __define_initcall(fn, 6)
#define driver_initcall_sync(fn)        __define_initcall(fn, 6s)
#define late_initcall(fn)               __define_initcall(fn, 7)
#define late_initcall_sync(fn)          __define_initcall(fn, 7s)
    
#define pre_console_initcall(fn)        ___define_initcall(fn, con, .console_initcall)
#define console_initcall(fn)            ___define_initcall(fn, con, .console_initcall)

extern void initcalls(void);

struct kernel_param {
	const char *str;
	state (*boot_func)(char *);
};

extern struct kernel_param _ld_boot_param_start[];
extern struct kernel_param _ld_boot_param_end[];

#define __boot_param(str, unique_id, fn)                \
        static struct kernel_param __boot_##unique_id   \
        __used __section(".init.param")                 \
        __aligned(__alignof__(struct kernel_param))     \
        = {str,fn}
#define boot_param(str, fn) __boot_param(str, fn, fn)

    
#endif /* _INITCALL_H_ */
