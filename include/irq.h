/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _IRQ_H_
#define _IRQ_H_

#include <state.h>
#include <list.h>
#include <rbtree.h>
#include <asm/irq.h>

typedef enum irq_return irqreturn_t;
typedef irqreturn_t (*irq_handler_t)(irqnr_t, void *);

enum irq_return {
    IRQ_RET_NONE        = 0x00,
    IRQ_RET_HANDLED     = 0x01,
    IRQ_RET_WAKE        = 0x02,
};

enum irq_flags {
    IRQ_FLAG_SHARED     = 0x00000000,
};

struct irq_desc {
    irq_handler_t handler;
    const char *name;
    void *data;
};

extern irqreturn_t irq_undefine_handle(irqnr_t vector, void *data);

extern void irq_handle(irqnr_t vector);
extern state irq_request(irqnr_t vector, enum irq_flags flags, irq_handler_t handler, void *data, const char *name);
extern void irq_release(irqnr_t vector);

#endif  /* _IRQ_H_ */
