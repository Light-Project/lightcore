/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _STATIC_CALL_H_
#define _STATIC_CALL_H_

#include <types.h>
#include <asm/rwonce.h>

#define STATIC_CALL_KEY_PREFIX          __SCK__
#define STATIC_CALL_KEY_PREFIX_STR      __stringify(STATIC_CALL_KEY_PREFIX)
#define STATIC_CALL_KEY_PREFIX_LEN      (sizeof(STATIC_CALL_KEY_PREFIX_STR) - 1)
#define STATIC_CALL_KEY(name)           __PASTE(STATIC_CALL_KEY_PREFIX, name)
#define STATIC_CALL_KEY_STR(name)       __stringify(STATIC_CALL_KEY(name))

#define STATIC_CALL_TRAMP_PREFIX        __SCT__
#define STATIC_CALL_TRAMP_PREFIX_STR    __stringify(STATIC_CALL_TRAMP_PREFIX)
#define STATIC_CALL_TRAMP_PREFIX_LEN    (sizeof(STATIC_CALL_TRAMP_PREFIX_STR) - 1)
#define STATIC_CALL_TRAMP(name)         __PASTE(STATIC_CALL_TRAMP_PREFIX, name)
#define STATIC_CALL_TRAMP_STR(name)     __stringify(STATIC_CALL_TRAMP(name))

#define DECLARE_STATIC_CALL(name, func)                     \
    extern struct static_call_key STATIC_CALL_KEY(name);    \
    extern typeof(func) STATIC_CALL_TRAMP(name);

#ifdef CONFIG_ARCH_HAS_STATIC_CALL_INLINE
struct static_call_key {
    void *func;
};
#else
struct static_call_key {
    void *func;
};
#endif

#if defined(CONFIG_ARCH_HAS_STATIC_CALL_INLINE)

#elif defined(CONFIG_ARCH_HAS_STATIC_CALL)

#else /* !CONFIG_ARCH_HAS_STATIC_CALL_INLINE */

#define __DEFINE_STATIC_CALL(name, _func, _func_init)       \
    DECLARE_STATIC_CALL(name, _func);                       \
    struct static_call_key STATIC_CALL_KEY(name) = {        \
        .func = _func_init,                                 \
    }

#define static_call(name) (             \
    (typeof(STATIC_CALL_TRAMP(name))*)  \
    (STATIC_CALL_KEY(name).func)        \
)

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

#define static_call_update(name, func) ({                   \
    typeof(&STATIC_CALL_TRAMP(name)) __func = (func);       \
    __static_call_update(&STATIC_CALL_KEY(name),            \
    STATIC_CALL_TRAMP_ADDR(name), __func);	                \
})

#define DEFINE_STATIC_CALL(name, func)                      \
    __DEFINE_STATIC_CALL(name, func, func)

#endif  /* _STATIC_CALL_H_ */
