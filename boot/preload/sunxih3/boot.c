#include "boot.h"
#include <chip/sunxih3.h>

static void led_on(void)
{
	PA->CFG2 = 0x1<<4;
	PA->DATA = 1<<17; 
}

static void led_off(void)
{
	PA->DATA &= ~(1<<17); 
}

static void delay(void)
{
	for(int i=0xfffff;i>0;i--);
}

int main()
{
    /* Set cpu clock*/
    pre_ccu_cpu(CONFIG_BOOTLOADER_CPU_CLK);

    /* Set system clock */
    pre_ccu_sys(CONFIG_BOOTLOADER_SYS_CLK);
    
    /* init memory control unit */
//     pre_dram_init(CONFIG_BOOTLOADER_DDR_CLK);
    
    while(1)
    {
        led_on();
        delay();
        led_off();
        delay();
    }
    
}
