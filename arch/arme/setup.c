#include <arch.h>
#include <types.h>
#include <size.h>
#include <init/initcall.h>

uint32_t arch_setup()
{
    arch_interrupt_init();
}
