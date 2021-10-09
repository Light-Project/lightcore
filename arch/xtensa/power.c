#include <asm/irq.h>
#include <power.h>

void power_halt()
{
    cpu_irq_disable();
    asm volatile ("waiti 0");
}

void power_reboot()
{
    cpu_irq_disable();
    asm volatile ("waiti 0");
}

void power_shutdown()
{
    cpu_irq_disable();
    asm volatile ("waiti 0");
}
