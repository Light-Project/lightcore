
#include <init/initcall.h>
#include <driver/platform.h>
#include <driver/watchdog.h>

state watchdog_feed(struct watchdog_device *wdev)
{
    state ret;

    /* If there is no feed function, restart the watchdog */
    if(wdev->ops->feed)
        ret = wdev->ops->feed(wdev);
    else
        ret = wdev->ops->start(wdev);

    return ret;
}
