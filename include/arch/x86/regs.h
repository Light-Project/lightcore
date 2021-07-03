
#ifndef _ARCH_REGS_H_
#define _ARCH_REGS_H_

#include <types.h>

typedef struct regs{
    uint32_t ebx;
    uint32_t ecx;
    uint32_t edx;
    uint32_t esi;
    uint32_t edi;
    uint32_t ebp;
    uint32_t eax;
    uint16_t ds;
    uint16_t es;
    uint16_t fs;
    uint16_t gs;
    uint32_t orig_eax;
    uint32_t eip;
    uint16_t cs;
    uint32_t eflags;
    uint32_t esp;
    uint16_t ss;
} __packed;

#endif
