/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _BOOT_H_
#define _BOOT_H_

#include <stddef.h>
#include <types.h>
#include <lib.h>

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

#define MACHINE_SCRATCH_ADDR    0x68000
#define MACHINE_SCRATCH_SEG     (MACHINE_SCRATCH_ADDR >> 4)
#define MACHINE_SCRATCH_SIZE    0x9000

#define load_seek 63
#define memory_top 0xa0000
extern unsigned char boot_dev;

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

#define BIGREAL_READ_OPS(name, type, size, reg)         \
static __always_inline type                             \
bigreal_generic_##name(const volatile void *addr)       \
{                                                       \
    type val;                                           \
    asm volatile (                                      \
        "mov"#size"  %%fs:%1, %0\n"                     \
        : reg(val) : "m" (*(volatile type *)addr)       \
        : "memory"                                      \
    );                                                  \
    return val;                                         \
}

#define BIGREAL_WRITE_OPS(name, type, size, reg)        \
static __always_inline void                             \
bigreal_generic_##name(volatile void *addr, type val)   \
{                                                       \
    asm volatile (                                      \
        "mov"#size" %1, %%fs:%0\n"                      \
        : "+m" (*(volatile type *)addr) : reg (val)     \
        : "memory"                                      \
    );                                                  \
}

BIGREAL_READ_OPS(readb, uint8_t, b, "=q")
BIGREAL_READ_OPS(readw, uint16_t, w, "=r")
BIGREAL_READ_OPS(readl, uint32_t, l, "=r")
BIGREAL_WRITE_OPS(writeb, uint8_t, b, "qi")
BIGREAL_WRITE_OPS(writew, uint16_t, w, "ri")
BIGREAL_WRITE_OPS(writel, uint32_t, l, "ri")

#define bigreal_readb       bigreal_generic_readb
#define bigreal_readw       bigreal_generic_readw
#define bigreal_readl       bigreal_generic_readl
#define bigreal_writeb      bigreal_generic_writeb
#define bigreal_writew      bigreal_generic_writew
#define bigreal_writel      bigreal_generic_writel

static inline void bigreal_memcpy(void *dest, const void *src, size_t len)
{
    uint8_t value;
    while (len--) {
        value = bigreal_readb(src++);
        bigreal_writeb(dest++, value);
    }
}

static inline void io_delay(void)
{
    asm volatile("outb %al, $0x80");
}

extern void setup_bigreal(void);
extern __noreturn void halt(void);
extern __noreturn void kboot_start(uint16_t segment, uint32_t offset);
extern void bios_int(char nr, struct bios_reg *ir, struct bios_reg *or);
extern void biosdisk_read(uint8_t dev, void *buff, uint32_t lba, int count);
extern void video_print(const char *str);
extern void video_clear(void);
extern void serial_print(const char *str);
extern void serial_init(void);
extern void a20_enable(void);

#endif  /* _BOOT_H_ */
