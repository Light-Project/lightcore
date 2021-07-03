#include <printk.h>
#include <types.h>
#include <stddef.h>
#include <arch/arme/nvic.h> 
#include <arch/arme/scb.h> 

extern size_t _ld_estack;
static size_t interrupt_entry[256] __aligned(256);

static void default_handle()
{
    printk("Undeclared interrupt");
}

void arch_interrupt_init()
{
    uint16_t count = 0;
    interrupt_entry[count++] = _ld_estack;
    interrupt_entry[count++] = (size_t)startup_head;
    for (; count <= ARRAY_SIZE(interrupt_entry); count++)
            interrupt_entry[count] = (size_t)default_handle;
    SCB->VTOR = (uint32_t)&interrupt_entry;
}

void arch_interrupt_set(uint8_t num, void *func)
{
    interrupt_entry[num] = (size_t)func;
}
