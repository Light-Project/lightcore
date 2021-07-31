/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _ASM_X86_IO_H_
#define _ASM_X86_IO_H_

#include <types.h>

void *ioremap(size_t port, size_t size);

#define inb inb
static inline uint8_t inb(uint16_t addr)
{
    uint8_t data = 0;
    asm volatile
    (
        "inb    %1, %0\n"
        :"=a"(data):"Nd"(addr):
    );
    return data;
}

#define inw inw
static inline uint16_t inw(uint16_t addr)
{
    uint16_t data = 0;
    asm volatile
    (
        "inw    %1, %0\n"
        :"=a"(data):"Nd"(addr):
    );
    return data;
}

#define inl inl
static inline uint32_t inl(uint16_t addr)
{
    uint32_t data = 0;
    asm volatile
    (
        "inl    %1, %0\n"
        :"=a"(data):"Nd"(addr):
    );
    return data;
}

#define outb outb
static inline void outb(uint16_t addr, uint8_t data)
{
    asm volatile
    (
        "outb   %1, %0\n"
        ::"Nd"(addr),"a"(data):
    );
}

#define outw outw
static inline void outw(uint16_t addr, uint16_t data)
{
    asm volatile
    (
        "outw   %1, %0\n"
        ::"Nd"(addr),"a"(data):
    );
}

#define outl outl
static inline void outl(uint16_t addr, uint32_t data)
{
    asm volatile
    (
        "outl   %1, %0\n"
        ::"Nd"(addr),"a"(data):
    );
}
 
#define IO_SPACE_LIMIT 0xffff

#include <asm-generic/io.h> 

#endif /* _ASM_X86_IO_H_ */
