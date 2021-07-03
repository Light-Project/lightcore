#include <stdarg.h>
#include <printk.h>
#include <system/irq.h>

state irq_alloc(irqnr_t nr, enum irq_flags flags, irq_handler_t fn, 
                void *data, const char *fmt, ...)
{
    va_list args;
    char buf[128];

    va_start(args, fmt);
    vsnprintf(buf, sizeof(buf), fmt, args);
    va_end(args);

    return -ENOERR;
}

void irq_free()
{
    
    
}
