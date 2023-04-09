/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2022 John Sanpe <sanpeqf@gmail.com>
 */

#ifndef _ASM_X86_SMI_H_
#define _ASM_X86_SMI_H_

#include <types.h>

struct smi_regs {
    uint32_t eax;
    uint32_t ebx;
    uint32_t ecx;
    uint32_t edx;
    uint32_t esi;
    uint32_t edi;
};

extern void smi_trigger(struct smi_regs *regs);

#endif /* _ASM_X86_SMI_H_ */
