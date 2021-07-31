#include <printk.h>
#include <init/init.h>

void terminal_logo(void)
{
    pr_notice("----------------------------------------------------\n");
    pr_notice(" _     _       _     _    ____                      \n");
    pr_notice("| |   (_) __ _| |__ | |_ / ___|___  _ __ ___        \n");
    pr_notice("| |   | |/ _` | '_ \\| __| |   / _ \\| '__/ _ \\    \n");
    pr_notice("| |___| | (_| | | | | |_| |__| (_) | | |  __/       \n");
    pr_notice("|_____|_|\\__, |_| |_|\\__|\\____\\___/|_|  \\___|  \n");
    pr_notice("         |___/                                      \n");
}
