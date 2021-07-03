#include <asm/irq.h>
#include <system/power.h>



void power_halt()
{
    arch_irq_disable();
    asm volatile("hlt");
}

void power_reboot()
{
    arch_irq_disable();
}

void power_shutdown()
{
    arch_irq_disable();
}
