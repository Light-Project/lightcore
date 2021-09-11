#include <asm/irq.h>
#include <power.h>

void power_halt()
{
    arch_irq_disable();
    asm volatile ("waiti 0");
}

void power_reboot()
{
    arch_irq_disable();
    asm volatile ("waiti 0");
}

void power_shutdown()
{
    arch_irq_disable();
    asm volatile ("waiti 0");
}
