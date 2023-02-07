/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2022 Sanpe <sanpeqf@gmail.com>
 */

#include <ioops.h>
#include <panic.h>
#include <export.h>

#ifndef ARCH_HAS_MPIO_SIZE
# define MPIO_MASK      0x0ffffUL
# define MPIO_OFFSET    0x10000UL
# define MPIO_RESERVED  0x80000UL
#endif

#define GENERIC_MPIO_OPS(mmio_ops, pmio_ops, addr) do { \
    resource_size_t port = (resource_size_t) addr;      \
    if (port >= MPIO_RESERVED) {                        \
        mmio_ops;                                       \
    } else if (port > MPIO_OFFSET) {                    \
        pmio_ops;                                       \
    } else {                                            \
        mpio_bad_access(#pmio_ops, port);               \
    }                                                   \
} while (0)

static void mpio_bad_access(const char *ops, resource_size_t port)
{
    DEBUG_DATA_CHECK(1, "Bad mpio access port %#lx (%s)\n", port, ops);
}

uint8_t mpio_readb(const void *addr)
{
    GENERIC_MPIO_OPS(return readb(addr), return inb(port), addr);
    return (uint8_t)~0UL;
}
EXPORT_SYMBOL(mpio_readb);

uint16_t mpio_readw(const void *addr)
{
    GENERIC_MPIO_OPS(return readw(addr), return inw(port), addr);
    return (uint16_t)~0UL;
}
EXPORT_SYMBOL(mpio_readw);

uint32_t mpio_readl(const void *addr)
{
    GENERIC_MPIO_OPS(return readl(addr), return inl(port), addr);
    return (uint32_t)~0UL;
}
EXPORT_SYMBOL(mpio_readl);

void mpio_writeb(void *addr, uint8_t val)
{
    GENERIC_MPIO_OPS(writeb(addr, val), outb(port, val), addr);
}
EXPORT_SYMBOL(mpio_writeb);

void mpio_writew(void *addr, uint16_t val)
{
    GENERIC_MPIO_OPS(writew(addr, val), outw(port, val), addr);
}
EXPORT_SYMBOL(mpio_writew);

void mpio_writel(void *addr, uint32_t val)
{
    GENERIC_MPIO_OPS(writel(addr, val), outl(port, val), addr);
}
EXPORT_SYMBOL(mpio_writel);

void *mpio_map(unsigned long port, unsigned int nr)
{
    if (port > MPIO_MASK)
        return NULL;
    return (void *)(uintptr_t)(MPIO_OFFSET + port);
}
EXPORT_SYMBOL(mpio_map);

void mpio_unmap(void *addr)
{
    /* Nothing */
}
EXPORT_SYMBOL(mpio_unmap);
