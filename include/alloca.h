/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#ifndef _ALLOCA_H
#define _ALLOCA_H

#include <types.h>
#include <string.h>

#define alloca __builtin_alloca
extern __malloc void *alloca(size_t size);

#define zalloca(size) ({        \
    void *stack;                \
    stack = alloca(size);       \
    memset(stack, 0, size);     \
    stack;                      \
})

#endif  /* _ALLOCA_H */
