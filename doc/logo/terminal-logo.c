#include <printk.h>
#include <init/init.h>

void terminal_logo(void)
{
    pre_printk("----------------------------------------------------\n");
    pre_printk(" _     _       _     _    ____                      \n");
    pre_printk("| |   (_) __ _| |__ | |_ / ___|___  _ __ ___        \n");
    pre_printk("| |   | |/ _` | '_ \\| __| |   / _ \\| '__/ _ \\    \n");
    pre_printk("| |___| | (_| | | | | |_| |__| (_) | | |  __/       \n");
    pre_printk("|_____|_|\\__, |_| |_|\\__|\\____\\___/|_|  \\___|  \n");
    pre_printk("         |___/                                      \n");
}
