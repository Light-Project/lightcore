
#include <arch/arme/interrupt.h>
#include <arch/arme/systick.h>

void lc_system_systimer_init()
{
    if(SYSTICK->CAL>>SYSTICK_CAL_SKEW)
        lc_system_crashed("SysTick can not find Reference");
    
    SYSTICK->REL = SYSTICK->CAL & 0x00FFFFFF;
    
    SysTick->CR = (1<<SYSTICK_CR_TICKINT | 1<<SYSTICK_CR_ENABLE);

}

void SysTick_Handler()
{
    
}

void system_timer_init()
{
    arch_interrupt_set(STK_INTERRUPT, SysTick_Handler);
    
    
}
