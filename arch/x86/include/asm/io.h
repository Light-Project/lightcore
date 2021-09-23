/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _ASM_X86_IO_H_
#define _ASM_X86_IO_H_

#include <types.h>

#define inb inb
static inline uint8_t inb(uint16_t port)
{
    uint8_t val;
    asm volatile (
        "inb    %1, %0\n"
        :"=a"(val):"Nd"(port):
    );
    return val;
}

#define inw inw
static inline uint16_t inw(uint16_t port)
{
    uint16_t val;
    asm volatile (
        "inw    %1, %0\n"
        :"=a"(val):"Nd"(port):
    );
    return val;
}

#define inl inl
static inline uint32_t inl(uint16_t port)
{
    uint32_t val;
    asm volatile (
        "inl    %1, %0\n"
        :"=a"(val):"Nd"(port):
    );
    return val;
}

#define outb outb
static inline void outb(uint16_t port, uint8_t val)
{
    asm volatile (
        "outb   %1, %0\n"
        ::"Nd"(port),"a"(val)
    );
}

#define outw outw
static inline void outw(uint16_t port, uint16_t val)
{
    asm volatile (
        "outw   %1, %0\n"
        ::"Nd"(port),"a"(val)
    );
}

#define outl outl
static inline void outl(uint16_t port, uint32_t val)
{
    asm volatile (
        "outl   %1, %0\n"
        ::"Nd"(port),"a"(val)
    );
}

#define insb insb
static inline void insb(uint16_t port, uint8_t *data, uint32_t count)
{
    asm volatile (
        "rep; insb (%%dx), %%es:(%%edi)\n"
        :"+c"(count), "+D"(data) :"d"(port) :"memory"
    );
}

#define insw insw
static inline void insw(uint16_t port, uint16_t *data, uint32_t count)
{
    asm volatile (
        "rep; insw (%%dx), %%es:(%%edi)\n"
        :"+c"(count), "+D"(data) :"d"(port) :"memory"
    );
}

#define insl insl
static inline void insl(uint16_t port, uint32_t *data, uint32_t count)
{
    asm volatile (
        "rep; insl (%%dx), %%es:(%%edi)\n"
        :"+c"(count), "+D"(data) :"d"(port) :"memory"
    );
}

#define outsb outsb
static inline void outsb(uint16_t port, uint8_t *data, uint32_t count)
{
    asm volatile (
        "rep; outsb %%es:(%%esi), (%%dx)\n"
        :"+c"(count), "+S"(data) :"d"(port) :"memory"
    );
}

#define outsw outsw
static inline void outsw(uint16_t port, uint16_t *data, uint32_t count)
{
    asm volatile (
        "rep; outsw %%es:(%%esi), (%%dx)\n"
        :"+c"(count), "+S"(data) :"d"(port) :"memory"
    );
}

#define outsl outsl
static inline void outsl(uint16_t port, uint32_t *data, uint32_t count)
{
    asm volatile (
        "rep; outsl %%es:(%%esi), (%%dx)\n"
        :"+c"(count), "+S"(data) :"d"(port) :"memory"
    );
}

#define IO_SPACE_LIMIT  0xffff

#include <asm-generic/io.h>

#endif /* _ASM_X86_IO_H_ */
