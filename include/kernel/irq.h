/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _KERNEL_IRQ_H_
#define _KERNEL_IRQ_H_

#include <state.h>
#include <list.h>
#include <asm/irq.h>

typedef enum irq_return irq_return_t;
typedef irq_return_t (*irq_handler_t)(irqnr_t, void *);

enum irq_flags {
    IRQ_FLAG_SHARED     = 0x00000000,
};

enum irq_return {
    IRQ_RET_NONE        = 0x00,
    IRQ_RET_HANDLED     = 0x01,
    IRQ_RET_WAKE        = 0x02,
};

struct irq {
    const char *name;

    irq_handler_t handler;
    void *data; /* handler para */
};

void irq_handle(irqnr_t vector);
state irq_request(irqnr_t nr, enum irq_flags flags, irq_handler_t handler, void *data, const char *name);
void irq_free(irqnr_t nr);

#endif  /* _KERNEL_IRQ_H_ */
