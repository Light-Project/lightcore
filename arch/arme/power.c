#include <power.h>


void power_shutdown(void)
{
    interrupt_mask();

}

void power_halt(void)
{
    interrupt_mask();


}

void power_reboot(void)
{
    interrupt_mask();


}
