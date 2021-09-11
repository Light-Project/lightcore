#include <asm/irq.h>
#include <power.h>



void power_halt()
{
    arch_irq_disable();
    asm volatile(
        "1: rep; hlt\n"
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
