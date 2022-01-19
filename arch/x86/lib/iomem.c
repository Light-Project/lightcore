/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <asm/io.h>
#include <export.h>

#define movs(type, dest, src)   \
asm volatile(                   \
    "movs" type                 \
    : "=&D"(dest), "=&S"(src)   \
    : "0"(dest), "1"(src)       \
    :"memory"                   \
)

static __always_inline void
rep_movs(void *dest, const volatile void *src, size_t size)
{
	unsigned long d0, d1, d2;

	asm volatile(
        "rep                \n"
        "movsl              \n"
        "testb  $2, %b4     \n"
        "je     1f          \n"
        "movsw              \n"
        "1:                 \n"
        "testb  $1, %b4     \n"
        "je     2f          \n"
        "movsb              \n"
        "2:                 \n"
        : "=&c"(d0), "=&D"(d1), "=&S"(d2)
        : "0"(size / 4), "q"(size), "1"((long)dest), "2"((long)src)
        : "memory"
    );
}

extern void memcpy_formio(void *dest, const volatile void *io, size_t size)
{
    if (unlikely(!size))
        return;

    if (unlikely((size_t)io & 0x01)) {
        movs("b", dest, io);
        size -= 1;
    }

    if (size > 1 && unlikely((size_t)io & 0x01)) {
        movs("w", dest, io);
        size -= 2;
    }

    rep_movs(dest, io, size);
}
EXPORT_SYMBOL(memcpy_formio);

extern void memcpy_toio(void *io, const volatile void *src, size_t size)
{

    if (unlikely((size_t)io & 0x01)) {
        movs("b", io, src);
        size -= 1;
    }

    if (size > 1 && unlikely((size_t)io & 0x01)) {
        movs("w", io, src);
        size -= 2;
    }

	rep_movs(io, src, size);
}
EXPORT_SYMBOL(memcpy_toio);
