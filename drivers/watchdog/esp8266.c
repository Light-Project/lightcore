/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <initcall.h>
#include <driver/platform.h>
#include <driver/watchdog.h>
#include <driver/watchdog/esp8266.h>
#include <asm/io.h>

static state esp8266_wdt_start(struct watchdog_device *wdev)
{
    writel((void *)(0x60000900 + WDT_ESP8266_CTL), WDT_ESP8266_CTL_EN | WDT_ESP8266_CTL_RSTLEN_MASK);
    return -ENOERR;
}

static state esp8266_wdt_feed(struct watchdog_device *wdev)
{
    writeb((void *)(0x60000900 + WDT_ESP8266_RST), WDT_ESP8266_RST_FEED_MASK);
    return -ENOERR;
}

static state esp8266_wdt_set_timeout(struct watchdog_device *wdev, uint time)
{
    writeb((void *)(0x60000900 + WDT_ESP8266_RST), WDT_ESP8266_RST_FEED_MASK);
    return -ENOERR;
}

static struct watchdog_ops esp8266_wdt_ops = {
    .start = esp8266_wdt_start,
    .feed = esp8266_wdt_feed,
    .set_timeout = esp8266_wdt_set_timeout,
};

static state esp8266_wdt_probe(struct platform_device *pdev, const void *pdata)
{
    struct watchdog_device *wdev;

    wdev = kzalloc(sizeof(*wdev), GFP_KERNEL);
    if (!wdev)
        return -ENOMEM;

    wdev->ops = &esp8266_wdt_ops;


    return watchdog_register(wdev);
}

static const struct dt_device_id esp8266_wdt_id[] = {
    { .compatible = "espressif,esp8266-wdt" },
    {},
};

static struct platform_driver esp8266_wdt_driver = {
    .driver = {
        .name = "esp8266-wdt",
    },
    .dt_table = esp8266_wdt_id,
    .probe = esp8266_wdt_probe,
};

static state esp8266_wdt_init(void)
{
    return platform_driver_register(&esp8266_wdt_driver);
}
driver_initcall(esp8266_wdt_init);
