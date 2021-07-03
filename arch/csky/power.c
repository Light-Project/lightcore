#include <asm/irq.h>
#include <system/power.h>



void power_halt()
{
    arch_irq_disable();
    asm volatile ("bkpt");
}

void power_reboot()
{
    arch_irq_disable();
    asm volatile ("bkpt");
}

void power_shutdown()
{
    arch_irq_disable();
    asm volatile ("bkpt");
}
