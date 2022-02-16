#include <irqflags.h>
#include <power.h>

void power_halt()
{
    irq_local_disable();
    asm volatile ("waiti 0");
}

void power_reboot()
{
    irq_local_disable();
    asm volatile ("waiti 0");
}

void power_shutdown()
{
    irq_local_disable();
    asm volatile ("waiti 0");
}
