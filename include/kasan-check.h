/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _KASAN_CHECK_H_
#define _KASAN_CHECK_H_

#include <types.h>
#include <stddef.h>

#ifndef CONFIG_KASAN

static inline bool kasan_check_read(const volatile void *addr, unsigned int size)
{
    return true;
}

static inline bool kasan_check_write(const volatile void *addr, unsigned int size)
{
    return true;
}

#else /* !CONFIG_KASAN */

extern bool kasan_check_read(const volatile void *addr, unsigned int size);
extern bool kasan_check_write(const volatile void *addr, unsigned int size);

#endif  /* CONFIG_KASAN */
#endif  /* _KASAN_CHECK_H_ */
