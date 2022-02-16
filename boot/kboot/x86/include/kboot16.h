/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _KBOOT16_H_
#define _KBOOT16_H_

#include <asm/bootparam.h>

extern struct bootparam bootparam;

struct bios_reg {
    union {
        struct {
            uint32_t edi;
            uint32_t esi;
            uint32_t ebp;
            uint32_t esp;
            uint32_t ebx;
            uint32_t edx;
            uint32_t ecx;
            uint32_t eax;
            uint32_t dses;
            uint32_t eflags;
        };
        struct {
            uint16_t di, hdi;
            uint16_t si, hsi;
            uint16_t bp, hbp;
            uint16_t sp, hsp;
            uint16_t bx, hbx;
            uint16_t dx, hdx;
            uint16_t cx, hcx;
            uint16_t ax, hax;
            uint16_t es, ds;
        };
        struct {
            uint8_t dil, dih, edi2, edi3;
            uint8_t sil, sih, esi2, esi3;
            uint8_t bpl, bph, ebp2, ebp3;
            uint8_t spl, sph, esp2, esp3;
            uint8_t bl, bh, ebx2, ebx3;
            uint8_t dl, dh, edx2, edx3;
            uint8_t cl, ch, ecx2, ecx3;
            uint8_t al, ah, eax2, eax3;
        };
    };
} __packed;

#define SEGMENT_READ_OPS(name, type, size, reg)         \
static __always_inline type                             \
segment_generic_##name(uint16_t segment,                \
                       const volatile void *addr)       \
{                                                       \
    type val;                                           \
    asm volatile (                                      \
        "movw       %2, %%gs    \n"                     \
        "mov"#size" %%gs:%1, %0 \n"                     \
        : reg(val)                                      \
        : "m" (*(volatile type *)addr), "r"(segment)    \
        : "memory"                                      \
    );                                                  \
    return val;                                         \
}

#define SEGMENT_WRITE_OPS(name, type, size, reg)        \
static __always_inline void                             \
segment_generic_##name(uint16_t segment,                \
                       volatile void *addr, type val)   \
{                                                       \
    asm volatile (                                      \
        "movw       %2, %%gs    \n"                     \
        "mov"#size" %1, %%gs:%0 \n"                     \
        : "+m" (*(volatile type *)addr)                 \
        : reg (val), "r"(segment)                       \
        : "memory"                                      \
    );                                                  \
}

SEGMENT_READ_OPS(readb, uint8_t, b, "=q")
SEGMENT_READ_OPS(readw, uint16_t, w, "=r")
SEGMENT_READ_OPS(readl, uint32_t, l, "=r")
SEGMENT_WRITE_OPS(writeb, uint8_t, b, "q")
SEGMENT_WRITE_OPS(writew, uint16_t, w, "r")
SEGMENT_WRITE_OPS(writel, uint32_t, l, "r")

#define segment_readb       segment_generic_readb
#define segment_readw       segment_generic_readw
#define segment_readl       segment_generic_readl
#define segment_writeb      segment_generic_writeb
#define segment_writew      segment_generic_writew
#define segment_writel      segment_generic_writel

static inline void io_delay(void)
{
    asm volatile("outb %al, $0x80");
}

extern void bios_int(char nr, struct bios_reg *in, struct bios_reg *out);

#endif  /* _KBOOT16_H_ */
