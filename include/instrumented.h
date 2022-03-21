/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _INSTRUMENTED_H_
#define _INSTRUMENTED_H_

#include <kasan-check.h>

static __always_inline void
instrument_read(const volatile void *addr, size_t size)
{
    kasan_check_read(addr, size);
}

static __always_inline void
instrument_write(const volatile void *addr, size_t size)
{
    kasan_check_write(addr, size);
}

static __always_inline void
instrument_read_write(const volatile void *addr, size_t size)
{
    kasan_check_write(addr, size);
}

static __always_inline void
instrument_atomic_read(const volatile void *addr, size_t size)
{
    kasan_check_read(addr, size);
}

static __always_inline void
instrument_atomic_write(const volatile void *addr, size_t size)
{
    kasan_check_write(addr, size);
}

static __always_inline void
instrument_atomic_read_write(const volatile void *addr, size_t size)
{
    kasan_check_write(addr, size);
}

#endif  /* _INSTRUMENTED_H_ */
