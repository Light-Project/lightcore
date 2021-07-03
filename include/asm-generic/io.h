#ifndef _ASM_GENERIC_IO_H_
#define _ASM_GENERIC_IO_H_

#include <types.h>
#include <asm/barrier.h>
#include <lightcore/asm/byteorder.h>

/*
 * raw_{read,write}{b,w,l,q}() access memory in native endianness.
 *
 * On some architectures memory mapped IO needs to be accessed differently.
 * On the simple architectures, we just read/write the memory location
 * directly.
 */
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
static inline void raw_writeb(const volatile void *addr, uint8_t value)
{
    *(volatile uint8_t *)addr = value;
}
#endif

#ifndef raw_writew
#define raw_writew raw_writew
static inline void raw_writew(const volatile void *addr, uint16_t value)
{
    *(volatile uint16_t *)addr = value;
}
#endif

#ifndef raw_writel
#define raw_writel raw_writel
static inline void raw_writel(const volatile void *addr, uint32_t value)
{
    *(volatile uint32_t *)addr = value;
}
#endif

#ifdef CONFIG_ARCH_64BIT
#ifndef raw_writeq
#define raw_writeq raw_writeq
static inline void raw_writeq(const volatile void *addr, uint64_t value)
{
    *(volatile uint64_t *)addr = value;
}
#endif
#endif /* CONFIG_ARCH_64BIT */

/*
 * {read,write}{b,w,l,q}() are like the regular version, but
 * are not guaranteed to provide ordering against spinlocks or memory
 * accesses.
 */
#ifndef readb
#define readb readb
static inline uint8_t readb(void *addr)
{
    return raw_readb(addr);
}
#endif

#ifndef readw
#define readw readw
static inline uint16_t readw(void *addr)
{
    return raw_readw(addr);
}
#endif

#ifndef readl
#define readl readl
static inline uint32_t readl(void *addr)
{
    return raw_readl(addr);
}
#endif

#ifdef CONFIG_ARCH_64BIT
#ifndef readq
#define readq readq
static inline uint64_t readq(void *addr)
{
    return raw_readq(addr);
}
#endif
#endif /* CONFIG_ARCH_64BIT */


#ifndef writeb
#define writeb writeb
static inline void writeb(void *addr, uint8_t value)
{
    raw_writeb(addr, value);
}
#endif

#ifndef writew
#define writew writew
static inline void writew(void *addr, uint16_t value)
{
    raw_writew(addr, value);
}
#endif

#ifndef writel
#define writel writel
static inline void writel(void *addr, uint32_t value)
{
    raw_writel(addr, value);
}
#endif


#ifdef CONFIG_ARCH_64BIT
#ifndef writeq
#define writeq writeq
static inline void writeq(void *addr, uint64_t value)
{
    raw_writeq(addr, value);
}
#endif
#endif /* CONFIG_ARCH_64BIT */

/*
 * {read,write}{b,w,l,q}_sync() access little endian memory and return result in
 * native endianness.
 */
#ifndef readb_sync
#define readb_sync readb_sync
static inline uint8_t readb_sync(void *addr)
{
    uint8_t val;
    barrier();
    val = raw_readb(addr);
    rmb();
    return val;
}
#endif

#ifndef readw_sync
#define readw_sync readw_sync
static inline uint16_t readw_sync(void *addr)
{
    uint16_t val;
    barrier();
    val = raw_readw(addr);
    rmb();
    return val;
}
#endif

#ifndef readl_sync
#define readl_sync readl_sync
static inline uint32_t readl_sync(void *addr)
{
    uint32_t val;
    barrier();
    val = raw_readl(addr);
    rmb();
    return val;
}
#endif

#ifdef CONFIG_ARCH_64BIT
#ifndef readq_sync
#define readq_sync readq_sync
static inline uint64_t readq_sync(void *addr)
{
    uint64_t val;
    barrier();
    val = raw_readq(addr);
    rmb();
    return val;
    
}
#endif
#endif /* CONFIG_ARCH_64BIT */

#ifndef writeb_sync
#define writeb_sync writeb_sync
static inline void writeb_sync(void *addr, uint8_t val)
{
    wmb();
    raw_writeb(addr, val);
}
#endif

#ifndef writew_sync
#define writew_sync writew_sync
static inline void writew_sync(void *addr, uint16_t val)
{
    wmb();
    raw_writew(addr, val);
}
#endif

#ifndef writel_sync
#define writel_sync writel_sync
static inline void writel_sync(void *addr, uint32_t val)
{
    wmb();
    raw_writel(addr, val);
}
#endif

#ifdef CONFIG_ARCH_64BIT
#ifndef writeq_sync
#define writeq_sync writeq_sync
static inline void writeq_sync(void *addr, uint64_t val)
{
    wmb();
    raw_writeq(addr, val);
}
#endif
#endif /* CONFIG_ARCH_64BIT */
/*
 * {read,write}s{b,w,l,q}() repeatedly access the same memory address in
 * native endianness in 8-, 16-, 32- or 64-bit chunks (@count times).
 */
#ifndef readsb
#define readsb readsb
static inline void readsb(const volatile void *addr, void *buffer,
              unsigned int count)
{
    if (count) {
        u8 *buf = (u8 *)buffer;

        do {
            u8 x = raw_readb(addr);
            *buf++ = x;
        } while (--count);
    }
}
#endif

#ifndef readsw
#define readsw readsw
static inline void readsw(const volatile void *addr, void *buffer,
              unsigned int count)
{
    if (count) {
        u16 *buf = (u16 *)buffer;

        do {
            u16 x = raw_readw(addr);
            *buf++ = x;
        } while (--count);
    }
}
#endif

#ifndef readsl
#define readsl readsl
static inline void readsl(const volatile void *addr, void *buffer,
              unsigned int count)
{
    if (count) {
        u32 *buf = (u32 *)buffer;

        do {
            u32 x = raw_readl(addr);
            *buf++ = x;
        } while (--count);
    }
}
#endif

#ifdef CONFIG_ARCH_64BIT
#ifndef readsq
#define readsq readsq
static inline void readsq(const volatile void *addr, void *buffer,
              unsigned int count)
{
    if (count) {
        u64 *buf = (u64 *)buffer;

        do {
            u64 x = (u64 *)raw_readq(addr);
            *buf++ = x;
        } while (--count);
    }
}
#endif
#endif /* CONFIG_ARCH_64BIT */

#ifndef writesb
#define writesb writesb
static inline void writesb(volatile void *addr, const void *buffer,
               unsigned int count)
{
    if (count) {
        const u8 *buf = (u8 *)buffer;

        do {
            raw_writeb(addr, *buf++);
        } while (--count);
    }
}
#endif

#ifndef writesw
#define writesw writesw
static inline void writesw(volatile void *addr, const void *buffer,
               unsigned int count)
{
    if (count) {
        const u16 *buf = (u16 *)buffer;

        do {
            raw_writew(addr, *buf++);
        } while (--count);
    }
}
#endif

#ifndef writesl
#define writesl writesl
static inline void writesl(volatile void *addr, const void *buffer,
               unsigned int count)
{
    if (count) {
        const u32 *buf = (u32 *)buffer;

        do {
            raw_writel(addr, *buf++);
        } while (--count);
    }
}
#endif

#ifdef CONFIG_ARCH_64BIT
#ifndef writesq
#define writesq writesq
static inline void writesq(volatile void *addr, const void *buffer,
               unsigned int count)
{
    if (count) {
        const u64 *buf = (u64 *)buffer;

        do {
            raw_writeq(addr, *buf++);
        } while (--count);
    }
}
#endif
#endif /* CONFIG_ARCH_64BIT */

#ifndef PCI_IOBASE
#define PCI_IOBASE ((void __iomem *)0)
#endif

#ifndef IO_SPACE_LIMIT
#define IO_SPACE_LIMIT 0xffff
#endif

/*
 * {in,out}{b,w,l}() access little endian I/O. {in,out}{b,w,l}_p() can be
 * implemented on hardware that needs an additional delay for I/O accesses to
 * take effect.
 */
#ifndef inb
#define inb inb
static inline u8 inb(unsigned long addr)
{
    u8 val;

    barrier();
    val = raw_readb((void *)PCI_IOBASE + addr);
    rmb();
    return val;
}
#endif

#ifndef inw
#define inw inw
static inline u16 inw(unsigned long addr)
{
    u16 val;

    barrier();
    val = raw_readw((void *)PCI_IOBASE + addr);
    rmb();
    return val;
}
#endif

#ifndef inl
#define inl inl
static inline u32 inl(unsigned long addr)
{
    u32 val;

    barrier();
    val = raw_readl((void *)PCI_IOBASE + addr);
    rmb();
    return val;
}
#endif

#ifndef outb
#define outb outb
static inline void outb(unsigned long addr, uint8_t value)
{
    wmb();
    raw_writeb((void *)PCI_IOBASE + addr, value);
}
#endif

#ifndef outw
#define outw outw
static inline void outw(unsigned long addr, u16 value)
{
    wmb();
    raw_writew((void *)PCI_IOBASE + addr, value);
}
#endif

#ifndef outl
#define outl outl
static inline void outl(unsigned long addr, u32 value)
{
    wmb();
    raw_writel((void *)PCI_IOBASE + addr, value);
}
#endif

#endif /* _ASM_GENERIC_IO_H_ */
