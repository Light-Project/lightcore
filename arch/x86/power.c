#include <asm/irq.h>
#include <system/power.h>



void power_halt()
{
    arch_irq_disable();
    asm volatile(
        "1: rep; hlt\n\r"
        "jmp 1b"
    );
}

void power_reboot()
{
    arch_irq_disable();
}

void power_shutdown()
{
    arch_irq_disable();
}
