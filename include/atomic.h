/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _ATOMIC_H_
#define _ATOMIC_H_

#include <types.h>
#include <instrumented.h>
#include <asm/atomic.h>

static __always_inline atomic_t atomic_read(atomic_t *atomic)
{
    instrument_atomic_read(atomic, sizeof(*atomic));
    return arch_atomic_read(atomic);
}

static __always_inline void atomic_write(atomic_t *atomic, atomic_t val)
{
    instrument_atomic_write(atomic, sizeof(*atomic));
    arch_atomic_write(atomic, val);
}

static __always_inline void atomic_add(atomic_t *atomic, atomic_t val)
{
    instrument_atomic_read_write(atomic, sizeof(*atomic));
    arch_atomic_add(atomic, val);
}

static __always_inline void atomic_sub(atomic_t *atomic, atomic_t val)
{
    instrument_atomic_read_write(atomic, sizeof(*atomic));
    arch_atomic_sub(atomic, val);
}

static __always_inline void atomic_and(atomic_t *atomic, atomic_t val)
{
    instrument_atomic_read_write(atomic, sizeof(*atomic));
    arch_atomic_and(atomic, val);
}

static __always_inline void atomic_or(atomic_t *atomic, atomic_t val)
{
    instrument_atomic_read_write(atomic, sizeof(*atomic));
    arch_atomic_or(atomic, val);
}

static __always_inline void atomic_xor(atomic_t *atomic, atomic_t val)
{
    instrument_atomic_read_write(atomic, sizeof(*atomic));
    arch_atomic_xor(atomic, val);
}

static __always_inline atomic_t atomic_fetch_add(atomic_t *atomic, atomic_t val)
{
    instrument_atomic_read_write(atomic, sizeof(*atomic));
    return arch_atomic_fetch_add(atomic, val);
}

static __always_inline atomic_t atomic_fetch_sub(atomic_t *atomic, atomic_t val)
{
    instrument_atomic_read_write(atomic, sizeof(*atomic));
    return arch_atomic_fetch_sub(atomic, val);
}

static __always_inline atomic_t atomic_fetch_and(atomic_t *atomic, atomic_t val)
{
    instrument_atomic_read_write(atomic, sizeof(*atomic));
    return arch_atomic_fetch_and(atomic, val);
}

static __always_inline atomic_t atomic_fetch_or(atomic_t *atomic, atomic_t val)
{
    instrument_atomic_read_write(atomic, sizeof(*atomic));
    return arch_atomic_fetch_or(atomic, val);
}

static __always_inline atomic_t atomic_fetch_xor(atomic_t *atomic, atomic_t val)
{
    instrument_atomic_read_write(atomic, sizeof(*atomic));
    return arch_atomic_fetch_xor(atomic, val);
}

static __always_inline atomic_t atomic_add_return(atomic_t *atomic, atomic_t val)
{
    instrument_atomic_read_write(atomic, sizeof(*atomic));
    return arch_atomic_add_return(atomic, val);
}

static __always_inline atomic_t atomic_sub_return(atomic_t *atomic, atomic_t val)
{
    instrument_atomic_read_write(atomic, sizeof(*atomic));
    return arch_atomic_sub_return(atomic, val);
}

static __always_inline atomic_t atomic_and_return(atomic_t *atomic, atomic_t val)
{
    instrument_atomic_read_write(atomic, sizeof(*atomic));
    return arch_atomic_and_return(atomic, val);
}

static __always_inline atomic_t atomic_or_return(atomic_t *atomic, atomic_t val)
{
    instrument_atomic_read_write(atomic, sizeof(*atomic));
    return arch_atomic_or_return(atomic, val);
}

static __always_inline atomic_t atomic_xor_return(atomic_t *atomic, atomic_t val)
{
    instrument_atomic_read_write(atomic, sizeof(*atomic));
    return arch_atomic_xor_return(atomic, val);
}

static __always_inline bool atomic_add_test(atomic_t *atomic, atomic_t val)
{
    instrument_atomic_read_write(atomic, sizeof(*atomic));
    return arch_atomic_add_test(atomic, val);
}

static __always_inline bool atomic_sub_test(atomic_t *atomic, atomic_t val)
{
    instrument_atomic_read_write(atomic, sizeof(*atomic));
    return arch_atomic_sub_test(atomic, val);
}

static __always_inline bool atomic_and_test(atomic_t *atomic, atomic_t val)
{
    instrument_atomic_read_write(atomic, sizeof(*atomic));
    return arch_atomic_and_test(atomic, val);
}

static __always_inline bool atomic_or_test(atomic_t *atomic, atomic_t val)
{
    instrument_atomic_read_write(atomic, sizeof(*atomic));
    return arch_atomic_or_test(atomic, val);
}

static __always_inline bool atomic_xor_test(atomic_t *atomic, atomic_t val)
{
    instrument_atomic_read_write(atomic, sizeof(*atomic));
    return arch_atomic_xor_test(atomic, val);
}

static __always_inline void atomic_inc(atomic_t *atomic)
{
    instrument_atomic_read_write(atomic, sizeof(*atomic));
    arch_atomic_inc(atomic);
}

static __always_inline void atomic_dec(atomic_t *atomic)
{
    instrument_atomic_read_write(atomic, sizeof(*atomic));
    arch_atomic_dec(atomic);
}

static __always_inline atomic_t atomic_fetch_inc(atomic_t *atomic)
{
    instrument_atomic_read_write(atomic, sizeof(*atomic));
    return arch_atomic_fetch_inc(atomic);
}

static __always_inline atomic_t atomic_fetch_dec(atomic_t *atomic)
{
    instrument_atomic_read_write(atomic, sizeof(*atomic));
    return arch_atomic_fetch_dec(atomic);
}

static __always_inline atomic_t atomic_inc_return(atomic_t *atomic)
{
    instrument_atomic_read_write(atomic, sizeof(*atomic));
    return arch_atomic_inc_return(atomic);
}

static __always_inline atomic_t atomic_dec_return(atomic_t *atomic)
{
    instrument_atomic_read_write(atomic, sizeof(*atomic));
    return arch_atomic_dec_return(atomic);
}

static __always_inline bool atomic_inc_test(atomic_t *atomic)
{
    instrument_atomic_read_write(atomic, sizeof(*atomic));
    return arch_atomic_dec_return(atomic);
}

static __always_inline bool atomic_dec_test(atomic_t *atomic)
{
    instrument_atomic_read_write(atomic, sizeof(*atomic));
    return arch_atomic_dec_test(atomic);
}

static __always_inline atomic_t atomic_xchg(atomic_t *atomic, atomic_t val)
{
    instrument_atomic_read_write(atomic, sizeof(*atomic));
    return arch_atomic_xchg(atomic, val);
}

static __always_inline atomic_t atomic_cmpxchg(atomic_t *atomic, atomic_t old, atomic_t val)
{
    instrument_atomic_read_write(atomic, sizeof(*atomic));
    return arch_atomic_cmpxchg(atomic, old, val);
}

static __always_inline bool atomic_try_cmpxchg(atomic_t *atomic, atomic_t *old, atomic_t val)
{
    instrument_atomic_read_write(atomic, sizeof(*atomic));
	instrument_atomic_read_write(old, sizeof(*old));
    return arch_atomic_try_cmpxchg(atomic, old, val);
}

#define xchg(ptr, ...) ({                                   \
    typeof(ptr) __ai_ptr = (ptr);                           \
    instrument_atomic_write(__ai_ptr, sizeof(*__ai_ptr));   \
    arch_xchg(__ai_ptr, __VA_ARGS__);                       \
})

#define cmpxchg(ptr, ...) ({                                \
    typeof(ptr) __ai_ptr = (ptr);                           \
    instrument_atomic_write(__ai_ptr, sizeof(*__ai_ptr));   \
    arch_cmpxchg(__ai_ptr, __VA_ARGS__);                    \
})

#define try_cmpxchg(ptr, oldp, ...) ({                      \
    typeof(ptr) __ai_ptr = (ptr);                           \
    typeof(oldp) __ai_oldp = (oldp);                        \
    instrument_atomic_write(__ai_ptr, sizeof(*__ai_ptr));   \
    instrument_atomic_write(__ai_oldp, sizeof(*__ai_oldp)); \
    arch_try_cmpxchg(__ai_ptr, __ai_oldp, __VA_ARGS__);     \
})

#endif /* _ATOMIC_H_ */
