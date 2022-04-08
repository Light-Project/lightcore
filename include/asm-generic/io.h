/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _ASM_GENERIC_IO_H_
#define _ASM_GENERIC_IO_H_

#include <limits.h>
#include <string.h>
#include <asm/barrier.h>
#include <lightcore/asm/byteorder.h>

#ifdef CONFIG_GENERIC_MPIO
#include <asm-generic/mpio.h>
#endif

#ifndef raw_readb
#define raw_readb raw_readb
static inline uint8_t raw_readb(const volatile void *addr)
{
    return *(volatile uint8_t *)addr;
}
#endif

#ifndef raw_readw
#define raw_readw raw_readw
static inline uint16_t raw_readw(const volatile void *addr)
{
    return *(volatile uint16_t *)addr;
}
#endif

#ifndef raw_readl
#define raw_readl raw_readl
static inline uint32_t raw_readl(const volatile void *addr)
{
    return *(volatile uint32_t *)addr;
}
#endif

#ifndef raw_readq
#define raw_readq raw_readq
static inline uint64_t raw_readq(const volatile void *addr)
{
    return *(volatile uint64_t *)addr;
}
#endif

#ifndef raw_writeb
#define raw_writeb raw_writeb
static inline void raw_writeb(volatile void *addr, uint8_t value)
{
    *(volatile uint8_t *)addr = value;
}
#endif

#ifndef raw_writew
#define raw_writew raw_writew
static inline void raw_writew(volatile void *addr, uint16_t value)
{
    *(volatile uint16_t *)addr = value;
}
#endif

#ifndef raw_writel
#define raw_writel raw_writel
static inline void raw_writel(volatile void *addr, uint32_t value)
{
    *(volatile uint32_t *)addr = value;
}
#endif

#ifdef CONFIG_ARCH_64BIT
#ifndef raw_writeq
#define raw_writeq raw_writeq
static inline void raw_writeq(volatile void *addr, uint64_t value)
{
    *(volatile uint64_t *)addr = value;
}
#endif
#endif /* CONFIG_ARCH_64BIT */

#ifndef readb_relax
#define readb_relax readb_relax
static inline uint8_t readb_relax(const volatile void *addr)
{
    return raw_readb(addr);
}
#endif

#ifndef readw_relax
#define readw_relax readw_relax
static inline uint16_t readw_relax(const volatile void *addr)
{
    return raw_readw(addr);
}
#endif

#ifndef readl_relax
#define readl_relax readl_relax
static inline uint32_t readl_relax(const volatile void *addr)
{
    return raw_readl(addr);
}
#endif

#ifdef CONFIG_ARCH_64BIT
#ifndef readq_relax
#define readq_relax readq_relax
static inline uint64_t readq_relax(const volatile void *addr)
{
    return raw_readq(addr);
}
#endif
#endif /* CONFIG_ARCH_64BIT */

#ifndef writeb_relax
#define writeb_relax writeb_relax
static inline void writeb_relax(volatile void *addr, uint8_t value)
{
    raw_writeb(addr, value);
}
#endif

#ifndef writew_relax
#define writew_relax writew_relax
static inline void writew_relax(volatile void *addr, uint16_t value)
{
    raw_writew(addr, value);
}
#endif

#ifndef writel_relax
#define writel_relax writel_relax
static inline void writel_relax(volatile void *addr, uint32_t value)
{
    raw_writel(addr, value);
}
#endif

#ifdef CONFIG_ARCH_64BIT
#ifndef writeq_relax
#define writeq_relax writeq_relax
static inline void writeq_relax(volatile void *addr, uint64_t value)
{
    raw_writeq(addr, value);
}
#endif
#endif /* CONFIG_ARCH_64BIT */


#ifndef readb
#define readb readb
static inline uint8_t readb(const volatile void *addr)
{
    uint8_t val;
    barrier();
    val = raw_readb(addr);
    rmb();
    return val;
}
#endif

#ifndef readw
#define readw readw
static inline uint16_t readw(const volatile void *addr)
{
    uint16_t val;
    barrier();
    val = raw_readw(addr);
    rmb();
    return val;
}
#endif

#ifndef readl
#define readl readl
static inline uint32_t readl(const volatile void *addr)
{
    uint32_t val;
    barrier();
    val = raw_readl(addr);
    rmb();
    return val;
}
#endif

#ifdef CONFIG_ARCH_64BIT
#ifndef readq
#define readq readq
static inline uint64_t readq(const volatile void *addr)
{
    uint64_t val;
    barrier();
    val = raw_readq(addr);
    rmb();
    return val;

}
#endif
#endif /* CONFIG_ARCH_64BIT */

#ifndef writeb
#define writeb writeb
static inline void writeb(volatile void *addr, uint8_t val)
{
    wmb();
    raw_writeb(addr, val);
}
#endif

#ifndef writew
#define writew writew
static inline void writew(volatile void *addr, uint16_t val)
{
    wmb();
    raw_writew(addr, val);
}
#endif

#ifndef writel
#define writel writel
static inline void writel(volatile void *addr, uint32_t val)
{
    wmb();
    raw_writel(addr, val);
}
#endif

#ifdef CONFIG_ARCH_64BIT
#ifndef writeq
#define writeq writeq
static inline void writeq(volatile void *addr, uint64_t val)
{
    wmb();
    raw_writeq(addr, val);
}
#endif
#endif /* CONFIG_ARCH_64BIT */

#ifndef readsb
#define readsb readsb
static inline void readsb(const volatile void *addr, void *buffer, unsigned int count)
{
    if (count) {
        uint8_t *buf = buffer;

        do {
            uint8_t x = raw_readb(addr);
            *buf++ = x;
        } while (--count);
    }
}
#endif

#ifndef readsw
#define readsw readsw
static inline void readsw(const volatile void *addr, void *buffer, unsigned int count)
{
    if (count) {
        uint16_t *buf = buffer;

        do {
            uint16_t x = raw_readw(addr);
            *buf++ = x;
        } while (--count);
    }
}
#endif

#ifndef readsl
#define readsl readsl
static inline void readsl(const volatile void *addr, void *buffer, unsigned int count)
{
    if (count) {
        uint32_t *buf = buffer;

        do {
            uint32_t x = raw_readl(addr);
            *buf++ = x;
        } while (--count);
    }
}
#endif

#ifdef CONFIG_ARCH_64BIT
#ifndef readsq
#define readsq readsq
static inline void readsq(const volatile void *addr, void *buffer, unsigned int count)
{
    if (count) {
        uint64_t *buf = buffer;

        do {
            uint64_t x = raw_readq(addr);
            *buf++ = x;
        } while (--count);
    }
}
#endif
#endif /* CONFIG_ARCH_64BIT */

#ifndef writesb
#define writesb writesb
static inline void writesb(volatile void *addr, const void *buffer, unsigned int count)
{
    if (count) {
        const uint8_t *buf = (uint8_t *)buffer;

        do {
            raw_writeb(addr, *buf++);
        } while (--count);
    }
}
#endif

#ifndef writesw
#define writesw writesw
static inline void writesw(volatile void *addr, const void *buffer, unsigned int count)
{
    if (count) {
        const uint16_t *buf = (uint16_t *)buffer;

        do {
            raw_writew(addr, *buf++);
        } while (--count);
    }
}
#endif

#ifndef writesl
#define writesl writesl
static inline void writesl(volatile void *addr, const void *buffer, unsigned int count)
{
    if (count) {
        const uint32_t *buf = (uint32_t *)buffer;

        do {
            raw_writel(addr, *buf++);
        } while (--count);
    }
}
#endif

#ifdef CONFIG_ARCH_64BIT
#ifndef writesq
#define writesq writesq
static inline void writesq(volatile void *addr, const void *buffer, unsigned int count)
{
    if (count) {
        const uint64_t *buf = (uint64_t *)buffer;

        do {
            raw_writeq(addr, *buf++);
        } while (--count);
    }
}
#endif
#endif /* CONFIG_ARCH_64BIT */

#ifndef IO_SPACE_BASE
# define IO_SPACE_BASE NULL
#endif

#ifndef IO_SPACE_LIMIT
# define IO_SPACE_LIMIT UINT16_MAX
#endif

#ifndef inb
#define inb inb
static inline uint8_t inb(unsigned long addr)
{
    uint8_t val;

    barrier();
    val = raw_readb((void *)IO_SPACE_BASE + addr);
    rmb();
    return val;
}
#endif

#ifndef inw
#define inw inw
static inline uint16_t inw(unsigned long addr)
{
    uint16_t val;

    barrier();
    val = raw_readw((void *)IO_SPACE_BASE + addr);
    rmb();
    return val;
}
#endif

#ifndef inl
#define inl inl
static inline uint32_t inl(unsigned long addr)
{
    uint32_t val;

    barrier();
    val = raw_readl((void *)IO_SPACE_BASE + addr);
    rmb();
    return val;
}
#endif

#ifndef outb
#define outb outb
static inline void outb(unsigned long addr, uint8_t value)
{
    wmb();
    raw_writeb((void *)IO_SPACE_BASE + addr, value);
}
#endif

#ifndef outw
#define outw outw
static inline void outw(unsigned long addr, uint16_t value)
{
    wmb();
    raw_writew((void *)IO_SPACE_BASE + addr, value);
}
#endif

#ifndef outl
#define outl outl
static inline void outl(unsigned long addr, uint32_t value)
{
    wmb();
    raw_writel((void *)IO_SPACE_BASE + addr, value);
}
#endif

#ifndef insb
#define insb insb
static inline void insb(unsigned long addr, void *buffer, unsigned int count)
{
    readsb(IO_SPACE_BASE + addr, buffer, count);
}
#endif

#ifndef insw
#define insw insw
static inline void insw(unsigned long addr, void *buffer, unsigned int count)
{
    readsw(IO_SPACE_BASE + addr, buffer, count);
}
#endif

#ifndef insl
#define insl insl
static inline void insl(unsigned long addr, void *buffer, unsigned int count)
{
    readsl(IO_SPACE_BASE + addr, buffer, count);
}
#endif

#ifndef outsb
#define outsb outsb
static inline void outsb(unsigned long addr, const void *buffer, unsigned int count)
{
    writesb(IO_SPACE_BASE + addr, buffer, count);
}
#endif

#ifndef outsw
#define outsw outsw
static inline void outsw(unsigned long addr, const void *buffer, unsigned int count)
{
    writesw(IO_SPACE_BASE + addr, buffer, count);
}
#endif

#ifndef outsl
#define outsl outsl
static inline void outsl(unsigned long addr, const void *buffer, unsigned int count)
{
    writesl(IO_SPACE_BASE + addr, buffer, count);
}
#endif

#ifndef readq
#define readq readq
static inline uint64_t readq(const volatile void *addr)
{
    uint32_t low, high;

    low = readl(addr);
    high = readl(addr);

    return ((uint64_t)high << 32) | low;
}
#endif

#ifndef writeq
#define writeq writeq
static inline void writeq(volatile void *addr, uint64_t val)
{
    writel(addr, val);
    writel(addr + 4, val >> 32);
}
#endif

#ifndef readq_relax
#define readq_relax readq_relax
static inline uint64_t readq_relax(const volatile void *addr)
{
    uint32_t low, high;

    low = readl(addr);
    high = readl(addr);

    return ((uint64_t)high << 32) | low;
}
#endif

#ifndef writeq_relax
#define writeq_relax writeq_relax
static inline void writeq_relax(volatile void *addr, uint64_t val)
{
    writel_relax(addr, val);
    writel_relax(addr + 4, val >> 32);
}
#endif

#ifndef CONFIG_GENERIC_MPIO
#ifndef mpio_readb
#define mpio_readb mpio_readb
static inline uint8_t mpio_readb(const void *addr)
{
    return readb(addr);
}
#endif

#ifndef mpio_readw
#define mpio_readw mpio_readw
static inline uint16_t mpio_readw(const void *addr)
{
    return readw(addr);
}
#endif

#ifndef mpio_readl
#define mpio_readl mpio_readl
static inline uint32_t mpio_readl(const void *addr)
{
    return readl(addr);
}
#endif

#ifndef mpio_writeb
#define mpio_writeb mpio_writeb
static inline void mpio_writeb(void *addr, uint8_t val)
{
    writeb(addr, val);
}
#endif

#ifndef mpio_writew
#define mpio_writew mpio_writew
static inline void mpio_writew(void *addr, uint16_t val)
{
    writew(addr, val);
}
#endif

#ifndef mpio_writel
#define mpio_writel mpio_writel
static inline void mpio_writel(void *addr, uint32_t val)
{
    writel(addr, val);
}
#endif

#ifndef mpio_map
#define mpio_map mpio_map
static inline void *mpio_map(unsigned long port, unsigned int nr)
{
    port &= IO_SPACE_LIMIT;
    return IO_SPACE_BASE + port;
}
#endif

#ifndef mpio_unmap
#define mpio_unmap mpio_unmap
static inline void mpio_unmap(void *addr)
{
    /* Nothing */
}
#endif
#endif  /* CONFIG_GENERIC_MPIO */

#ifndef CONFIG_ARCH_HAS_PMIO
# define ioreadb(addr) readb(addr)
# define ioreadw(addr) readw(addr)
# define ioreadl(addr) readl(addr)
# define iowriteb(addr, val) writeb(addr, val)
# define iowritew(addr, val) writew(addr, val)
# define iowritel(addr, val) writel(addr, val)
#else /* !CONFIG_ARCH_HAS_PMIO */
# define ioreadb(addr) inb((unsigned long)addr)
# define ioreadw(addr) inw((unsigned long)addr)
# define ioreadl(addr) inl((unsigned long)addr)
# define iowriteb(addr, val) outb((unsigned long)addr, val)
# define iowritew(addr, val) outw((unsigned long)addr, val)
# define iowritel(addr, val) outl((unsigned long)addr, val)
#endif  /* CONFIG_ARCH_HAS_PMIO */

#ifndef memset_io
#define memset_io memset_io
static inline void memset_io(void *addr, int value, size_t size)
{
	memset(addr, value, size);
}
#endif

#ifndef memcpy_formio
#define memcpy_formio memcpy_formio
static inline void memcpy_formio(void *dest, const void *io, size_t size)
{
    memcpy(dest, io, size);
}
#endif

#ifndef memcpy_toio
#define memcpy_toio memcpy_toio
static inline void memcpy_toio(void *io, const void *src, size_t size)
{
    memcpy(io, src, size);
}
#endif

#endif /* _ASM_GENERIC_IO_H_ */
