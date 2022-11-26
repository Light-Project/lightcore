/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _ASM_GENERIC_GPIO_H_
#define _ASM_GENERIC_GPIO_H_

#include <types.h>
#include <state.h>

#ifndef ARCH_GPIO_LIMIT
# if defined(CONFIG_ARCH_GPIO_LIMIT)
#  define ARCH_GPIO_LIMIT CONFIG_ARCH_GPIO_LIMIT
# else
#  define ARCH_GPIO_LIMIT 512
# endif
#endif

static inline bool gpio_valid(unsigned int number)
{
    return number < ARCH_GPIO_LIMIT;
}

#endif  /* _ASM_GENERIC_GPIO_H_ */
