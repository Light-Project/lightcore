#include <printk.h>
#include <init/init.h>

void src_terminal_logo()
{
    pre_printk("----------------------------------------------------\n\r");
    pre_printk(" _     _       _     _    ____                      \n\r");
    pre_printk("| |   (_) __ _| |__ | |_ / ___|___  _ __ ___        \n\r");
    pre_printk("| |   | |/ _` | '_ \\| __| |   / _ \\| '__/ _ \\    \n\r");
    pre_printk("| |___| | (_| | | | | |_| |__| (_) | | |  __/       \n\r");
    pre_printk("|_____|_|\\__, |_| |_|\\__|\\____\\___/|_|  \\___|  \n\r");
    pre_printk("         |___/                                      \n\r");
    pre_printk("version: %s\n\r", CONFIG_VERSION);
    pre_printk("kernel command: %s\n\r", boot_args);
}
