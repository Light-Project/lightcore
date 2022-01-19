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
            uint32_t _esp;
            uint32_t ebx;
            uint32_t edx;
            uint32_t ecx;
            uint32_t eax;
            uint32_t _fsgs;
            uint32_t _dses;
            uint32_t eflags;
        };
        struct {
            uint16_t di, hdi;
            uint16_t si, hsi;
            uint16_t bp, hbp;
            uint16_t _sp, _hsp;
            uint16_t bx, hbx;
            uint16_t dx, hdx;
            uint16_t cx, hcx;
            uint16_t ax, hax;
            uint16_t gs, fs;
            uint16_t es, ds;
            uint16_t flags, hflags;
        };
        struct {
            uint8_t dil, dih, edi2, edi3;
            uint8_t sil, sih, esi2, esi3;
            uint8_t bpl, bph, ebp2, ebp3;
            uint8_t _spl, _sph, _esp2, _esp3;
            uint8_t bl, bh, ebx2, ebx3;
            uint8_t dl, dh, edx2, edx3;
            uint8_t cl, ch, ecx2, ecx3;
            uint8_t al, ah, eax2, eax3;
        };
    };
} __packed;

extern void bios_int(char nr, struct bios_reg *in, struct bios_reg *out);

#endif  /* _KBOOT16_H_ */
