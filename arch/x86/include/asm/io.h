/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _ASM_X86_IO_H_
#define _ASM_X86_IO_H_

#include <types.h>

extern void io_delay(void);

#define MMIO_READ_OPS(name, type, size, reg, barrier)   \
static __always_inline type                             \
generic_##name(const volatile void *addr)               \
{                                                       \
    type val;                                           \
    asm volatile (                                      \
        "mov"#size"  %1, %0\n"                          \
        : reg(val) : "m" (*(volatile type *)addr)       \
        : barrier                                       \
    );                                                  \
    return val;                                         \
}

#define MMIO_WRITE_OPS(name, type, size, reg, barrier)  \
static __always_inline void                             \
generic_##name(volatile void *addr, type val)           \
{                                                       \
    asm volatile (                                      \
        "mov"#size" %0, %1\n"                           \
        :: reg (val), "m" (*(volatile type *)addr)      \
        : barrier                                       \
    );                                                  \
}

MMIO_READ_OPS(readb, uint8_t, b, "=q", "memory")
MMIO_READ_OPS(readw, uint16_t, w, "=r", "memory")
MMIO_READ_OPS(readl, uint32_t, l, "=r", "memory")
MMIO_WRITE_OPS(writeb, uint8_t, b, "q", "memory")
MMIO_WRITE_OPS(writew, uint16_t, w, "r", "memory")
MMIO_WRITE_OPS(writel, uint32_t, l, "r", "memory")
MMIO_READ_OPS(readb_relax, uint8_t, b, "=q", )
MMIO_READ_OPS(readw_relax, uint16_t, w, "=r", )
MMIO_READ_OPS(readl_relax, uint32_t, l, "=r", )
MMIO_WRITE_OPS(writeb_relax, uint8_t, b, "q", )
MMIO_WRITE_OPS(writew_relax, uint16_t, w, "r", )
MMIO_WRITE_OPS(writel_relax, uint32_t, l, "r", )

#define readb           generic_readb
#define readw           generic_readw
#define readl           generic_readl
#define writeb          generic_writeb
#define writew          generic_writew
#define writel          generic_writel
#define readb_relax     generic_readb_relax
#define readw_relax     generic_readw_relax
#define readl_relax     generic_readl_relax
#define writeb_relax    generic_writeb_relax
#define writew_relax    generic_writew_relax
#define writel_relax    generic_writel_relax

#define IOPORT_IN_OPS(name, type)           \
static __always_inline type                 \
generic_##name(uint16_t port)               \
{                                           \
    type val;                               \
    asm volatile (                          \
        #name"  %1, %0\n"                   \
        :"=a"(val):"Nd"(port):              \
    );                                      \
    return val;                             \
}

#define IOPORT_OUT_OPS(name, type)          \
static __always_inline void                 \
generic_##name(uint16_t port, type val)     \
{                                           \
    asm volatile (                          \
        #name"  %1, %0\n"                   \
        ::"Nd"(port),"a"(val)               \
    );                                      \
}

#define IOPORT_INP_OPS(name, type)          \
static __always_inline type                 \
generic_##name##_p(uint16_t port)           \
{                                           \
    type val;                               \
    asm volatile (                          \
        #name"  %1, %0\n"                   \
        :"=a"(val):"Nd"(port):              \
    );                                      \
    io_delay();                             \
    return val;                             \
}

#define IOPORT_OUTP_OPS(name, type)         \
static __always_inline void                 \
generic_##name##_p(uint16_t port, type val) \
{                                           \
    asm volatile (                          \
        #name"  %1, %0\n"                   \
        ::"Nd"(port),"a"(val)               \
    );                                      \
    io_delay();                             \
}

#define IOPORT_INS_OPS(name, type)          \
static __always_inline void                 \
generic_##name(uint16_t port, type *data,   \
               uint32_t count)              \
{                                           \
    asm volatile (                          \
        "rep\n"                             \
        #name"  (%%dx), %%es:(%%edi)\n"     \
        : "+c"(count), "+D"(data)           \
        : "d"(port) : "memory"              \
    );                                      \
}

#define IOPORT_OUTS_OPS(name, type)         \
static __always_inline void                 \
generic_##name(uint16_t port, type *data,   \
               uint32_t count)              \
{                                           \
    asm volatile (                          \
        "rep\n"                             \
        #name"  %%es:(%%esi), (%%dx)\n"     \
        : "+c"(count), "+S"(data)           \
        : "d"(port) : "memory"              \
    );                                      \
}

IOPORT_IN_OPS(inb, uint8_t)
IOPORT_IN_OPS(inw, uint16_t)
IOPORT_IN_OPS(inl, uint32_t)
IOPORT_OUT_OPS(outb, uint8_t)
IOPORT_OUT_OPS(outw, uint16_t)
IOPORT_OUT_OPS(outl, uint32_t)
IOPORT_INP_OPS(inb, uint8_t)
IOPORT_INP_OPS(inw, uint16_t)
IOPORT_INP_OPS(inl, uint32_t)
IOPORT_OUTP_OPS(outb, uint8_t)
IOPORT_OUTP_OPS(outw, uint16_t)
IOPORT_OUTP_OPS(outl, uint32_t)
IOPORT_INS_OPS(insb, uint8_t)
IOPORT_INS_OPS(insw, uint16_t)
IOPORT_INS_OPS(insl, uint32_t)
IOPORT_OUTS_OPS(outsb, uint8_t)
IOPORT_OUTS_OPS(outsw, uint16_t)
IOPORT_OUTS_OPS(outsl, uint32_t)

#define inb     generic_inb
#define inw     generic_inw
#define inl     generic_inl
#define outb    generic_outb
#define outw    generic_outw
#define outl    generic_outl
#define inb_p   generic_inb_p
#define inw_p   generic_inw_p
#define inl_p   generic_inl_p
#define outb_p  generic_outb_p
#define outw_p  generic_outw_p
#define outl_p  generic_outl_p
#define insb    generic_insb
#define insw    generic_insw
#define insl    generic_insl
#define outsb   generic_outsb
#define outsw   generic_outsw
#define outsl   generic_outsl

#define memcpy_formio memcpy_formio
#define memcpy_toio memcpy_toio
extern void memcpy_formio(void *dest, const volatile void *io, size_t size);
extern void memcpy_toio(void *io, const volatile void *src, size_t size);

#include <asm-generic/io.h>

#endif /* _ASM_X86_IO_H_ */
