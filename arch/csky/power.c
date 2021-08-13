#include <asm/irq.h>
#include <kernel/power.h>

void power_halt()
{
    arch_irq_disable();
    asm volatile ("wait");
}

void power_reboot()
{
    power_halt();
}

void power_shutdown()
{
    power_halt();
}
