/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _ARCH_X86_TSS_H_
#define _ARCH_X86_TSS_H_

#ifndef __ASSEMBLY__

#include <types.h>
#include <kernel.h>

#ifndef CONFIG_X86_64

struct tss_entry {
    uint16_t    link, linkh;
    uint32_t    esp0;
    uint16_t    ss0, ss0h;
    uint32_t    esp1;
    uint16_t    ss1;
    uint16_t    ss1h;
    uint32_t    esp2;
    uint16_t    ss2, ss2h;
    uint32_t    cr3;
    uint32_t    eip;
    uint32_t    eflags;
    uint32_t    eax;
    uint32_t    ecx;
    uint32_t    edx;
    uint32_t    ebx;
    uint32_t    esp;
    uint32_t    ebp;
    uint32_t    esi;
    uint32_t    edi;
    uint16_t    es, esh;
    uint16_t    cs, csh;
    uint16_t    ss, ssh;
    uint16_t    ds, dsh;
    uint16_t    fs, fsh;
    uint16_t    gs, gsh;
    uint16_t    ldt, ldth;
    uint16_t    trace;
    uint16_t    iomap_base;
} __packed;

#else   /* !CONFIG_X86_64 */

struct tss_entry {
    uint32_t    res1;
    uint64_t    rsp0;
    uint64_t    rsp1;
    uint64_t    rsp2;
    uint64_t    res2;
    uint64_t    ist[7];
    uint32_t    res3;
    uint32_t    res4;
    uint16_t    res5;
    uint16_t    iomap_base;
} __packed;

#endif /* CONFIG_X86_64 */
#endif /* __ASSEMBLY__ */

#define IO_BITMAP_BITS      65536
#define IO_BITMAP_BYTES     (IO_BITMAP_BITS / 8)
#define IO_BITMAP_LONGS     (IO_BITMAP_BYTES / MSIZE)

#endif /* _ARCH_X86_TSS_H_ */
