/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _STATIC_CALL_H_
#define _STATIC_CALL_H_

#include <types.h>

#define STATIC_CALL_KEY_PREFIX          __STATIC_CALL_KEY__
#define STATIC_CALL_KEY_PREFIX_STR      __stringify(STATIC_CALL_KEY_PREFIX)
#define STATIC_CALL_KEY_PREFIX_LEN      (sizeof(STATIC_CALL_KEY_PREFIX_STR) - 1)
#define STATIC_CALL_KEY(name)           __PASTE(STATIC_CALL_KEY_PREFIX, name)
#define STATIC_CALL_KEY_STR(name)       __stringify(STATIC_CALL_KEY(name))

#define STATIC_CALL_TRAMP_PREFIX        __STATIC_CALL_TRAMP__
#define STATIC_CALL_TRAMP_PREFIX_STR    __stringify(STATIC_CALL_TRAMP_PREFIX)
#define STATIC_CALL_TRAMP_PREFIX_LEN    (sizeof(STATIC_CALL_TRAMP_PREFIX_STR) - 1)
#define STATIC_CALL_TRAMP(name)         __PASTE(STATIC_CALL_TRAMP_PREFIX, name)
#define STATIC_CALL_TRAMP_STR(name)     __stringify(STATIC_CALL_TRAMP(name))

#ifdef CONFIG_ARCH_HAS_STATIC_CALL_INLINE
struct static_call_key {
    void *func;
};
#else
struct static_call_key {
    void *func;
};
#endif

#define DECLARE_STATIC_CALL(name, func)                     \
    extern struct static_call_key STATIC_CALL_KEY(name);    \
    extern typeof(func) STATIC_CALL_TRAMP(name);

#if defined(CONFIG_ARCH_HAS_STATIC_CALL_INLINE)

#elif defined(CONFIG_ARCH_HAS_STATIC_CALL)

#else /* !CONFIG_ARCH_HAS_STATIC_CALL_INLINE */

#define __DEFINE_STATIC_CALL(name, _func, _func_init)       \
    DECLARE_STATIC_CALL(name, _func);                       \
    struct static_call_key STATIC_CALL_KEY(name) = {        \
        .func = _func_init,                                 \
    }

#define __static_call_cond(name) ({                         \
    void *func = READ_ONCE(STATIC_CALL_KEY(name).func);     \
    if (!func)                                              \
        func = &__static_call_nop;                          \
    (typeof(STATIC_CALL_TRAMP(name))*)func;                 \
})

static inline void __static_call_nop(void)
{
    /* Nothing */
}

static inline int __static_call_ret0(void)
{
    return 0;
}

static __always_inline void
__static_call_update(struct static_call_key *key, void *tramp, void *func)
{
    WRITE_ONCE(key->func, func);
}

#endif

#ifdef CONFIG_ARCH_HAS_STATIC_CALL
# define STATIC_CALL_TRAMP_ADDR(name) &STATIC_CALL_TRAMP(name)
#else
# define STATIC_CALL_TRAMP_ADDR(name) NULL
#endif

#define static_call_cond(name)                              \
    (void)__static_call_cond(name)

#define static_call_update(name, func) ({                   \
    typeof(&STATIC_CALL_TRAMP(name)) __func = (func);       \
    __static_call_update(&STATIC_CALL_KEY(name),            \
    STATIC_CALL_TRAMP_ADDR(name), __func);	                \
})

#define DEFINE_STATIC_CALL(name, func)                      \
    __DEFINE_STATIC_CALL(name, func, func)

#define DEFINE_STATIC_CALL_RET(name, func)                  \
    __DEFINE_STATIC_CALL(name, func, __static_call_nop)

#define DEFINE_STATIC_CALL_RET0(name, func)                 \
    __DEFINE_STATIC_CALL(name, func, __static_call_ret0)

#endif /* _STATIC_CALL_H_ */
