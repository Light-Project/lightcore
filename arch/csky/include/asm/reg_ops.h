/* SPDX-License-Identifier: GPL-2.0 */

#ifndef _ASM_REGS_OPS_H_
#define _ASM_REGS_OPS_H_

#define mfcr(reg)       \
({                      \
    unsigned int tmp;   \
    asm volatile(       \
    "mfcr %0, "reg"\n"  \
    : "=r"(tmp)         \
    :                   \
    : "memory");        \
    tmp;                \
})

#define mtcr(reg, val)		\
({				\
	asm volatile(		\
	"mtcr %0, "reg"\n"	\
	:			\
	: "r"(val)		\
	: "memory");		\
})

#endif /* __ASM_REGS_OPS_H */
