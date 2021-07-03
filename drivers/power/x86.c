
static void kb_restart(void)
{
    outb(0x64, 0xfe);
}

static void x86_restart(void)
{
    kb_restart();
}

static void x86_poweroff(void)
{
    
}

static struct power_ops x86_ops = {
    .restart = x86_restart,
    .poweroff = x86_poweroff,
}
