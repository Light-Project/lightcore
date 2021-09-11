
#include <initcall.h>
#include <driver/platform.h>
#include <driver/watchdog.h>

LIST_HEAD(watchdog_list);

void watchdog_daemon(void)
{
    struct watchdog_device *wdev;
    list_for_each_entry(wdev, &watchdog_list, list) {
        watchdog_feed(wdev);
    }
}


state watchdog_register(struct watchdog_device *wdev)
{


    return -ENOERR;
}


static __init state watchdog_init(void)
{
    // kthread_create(watchdog_daemon, 0, "watchdogd");
    return -ENOERR;
}

framework_initcall(watchdog_init);
