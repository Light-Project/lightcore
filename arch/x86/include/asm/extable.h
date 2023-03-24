/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _ASM_X86_EXTABLE_H_
#define _ASM_X86_EXTABLE_H_

#define EXTABLE_TYPE_SHIFT          0
#define EXTABLE_REG_SHIFT           8
#define EXTABLE_FLAG_SHIFT          12
#define EXTABLE_IMM_SHIFT           16

#define EXTABLE_TYPE_MASK           0x000000ff
#define EXTABLE_REG_MASK            0x00000f00
#define EXTABLE_FLAG_MASK           0x0000f000
#define EXTABLE_IMM_MASK            0xffff0000

/* extable types */
#define	EXTABLE_TYPE_NONE           (0  << EXTABLE_TYPE_SHIFT)
#define	EXTABLE_TYPE_DEFAULT        (1  << EXTABLE_TYPE_SHIFT)
#define	EXTABLE_TYPE_FAULT          (2  << EXTABLE_TYPE_SHIFT)
#define	EXTABLE_TYPE_UACCESS        (3  << EXTABLE_TYPE_SHIFT)
#define	EXTABLE_TYPE_COPY           (4  << EXTABLE_TYPE_SHIFT)
#define	EXTABLE_TYPE_CLEAR_FS       (5  << EXTABLE_TYPE_SHIFT)
#define	EXTABLE_TYPE_FPU_RESTORE    (6  << EXTABLE_TYPE_SHIFT)
#define	EXTABLE_TYPE_BPF            (7  << EXTABLE_TYPE_SHIFT)
#define	EXTABLE_TYPE_RDMSR          (8  << EXTABLE_TYPE_SHIFT)
#define	EXTABLE_TYPE_WRMSR          (9  << EXTABLE_TYPE_SHIFT)
#define	EXTABLE_TYPE_RDMSR_SAFE     (10 << EXTABLE_TYPE_SHIFT)
#define	EXTABLE_TYPE_WRMSR_SAFE     (11 << EXTABLE_TYPE_SHIFT)

/* extable flags */
#define	EXTABLE_FLAG_CLEAR_AX       (1 << EXTABLE_FLAG_SHIFT)
#define	EXTABLE_FLAG_CLEAR_DX       (2 << EXTABLE_FLAG_SHIFT)
#define	EXTABLE_FLAG_CLEAR_AXDX     (3 << EXTABLE_FLAG_SHIFT)

#include <asm-generic/extable.h>

#endif /* _ASM_X86_EXTABLE_H_ */
