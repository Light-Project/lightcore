

void cpu_irq_disable()
{
    asm volatile("psrclr ie\n":::"memory");
}

void cpu_irq_enable()
{
    asm volatile("psrset ee, ie\n":::"memory");
}

void arch_cpu_idle()
{
#ifdef CONFIG_CPU_PM_WAIT
    asm volatile("wait\n");
#endif

#ifdef CONFIG_CPU_PM_DOZE
    asm volatile("doze\n");
#endif

#ifdef CONFIG_CPU_PM_STOP
    asm volatile("stop\n");
#endif
}
