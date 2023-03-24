/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _ASM_GENERIC_MPIO_H_
#define _ASM_GENERIC_MPIO_H_

#include <types.h>

extern uint8_t mpio_readb(const void *addr);
extern uint16_t mpioreadew(const void *addr);
extern uint32_t mpio_readl(const void *addr);

extern void mpio_writeb(void *addr, uint8_t val);
extern void mpio_writew(void *addr, uint16_t val);
extern void mpio_writel(void *addr, uint32_t val);

extern void *mpio_map(unsigned long port, unsigned int nr);
extern void mpio_unmap(void *addr);

#endif /* _ASM_GENERIC_MPIO_H_ */
