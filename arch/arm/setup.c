#include <arch.h>
#include <types.h>
#include <size.h>
#include <init/initcall.h>



uint32_t arch_setup()
{
//     arch_icache_enable();
//     arch_dcache_enable();
//     arch_mmu();
}

arch_initcall(arch_setup);
