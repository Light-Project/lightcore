
#include <state.h>
#include <asm/irq.h>

enum irq_flags {
    IRQ_FLAG_SHARED     = 0x00000000,
};

typedef enum irq_return {
    IRQ_RET_NONE        = 0x00,
    IRQ_RET_HANDLED     = 0x01,
    IRQ_RET_WAKE        = 0x02,
} irq_return_t;

typedef irq_return_t (*irq_handler_t)(int, void *);

state irq_alloc(irqnr_t nr, enum irq_flags flags, irq_handler_t fn, 
                void *data, const char *name, ...);
