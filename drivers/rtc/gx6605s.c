/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#define DRIVER_NAME "gx6605s-rtc"

#include <initcall.h>
#include <driver/platform.h>
#include <driver/rtc.h>
#include <driver/rtc/gx6605s.h>
#include <asm/io.h>

struct gx6605s_device {
    struct rtc_device rtc;
    void *base;
};

#define rtc_to_gx6605s(wdg) \
    container_of(wdg, struct gx6605s_device, rtc)

static __always_inline uint32_t
gx6605s_read(struct gx6605s_device *gdev, unsigned int reg)
{
    return readl(gdev->base + reg);
}

static __always_inline void
gx6605s_write(struct gx6605s_device *gdev, unsigned int reg, uint32_t val)
{
    writel(gdev->base + reg, val);
}

static state gx6605s_rtc_gettime(struct rtc_device *rtc, struct rtc_time *time)
{
    struct gx6605s_device *gdev = rtc_to_gx6605s(rtc);

    time->tm_us   = gx6605s_read(gdev, GX6605S_RTC_US);
    time->tm_ms   = gx6605s_read(gdev, GX6605S_RTC_MS);
    time->tm_sec  = gx6605s_read(gdev, GX6605S_RTC_SEC);
    time->tm_min  = gx6605s_read(gdev, GX6605S_RTC_MIN);
    time->tm_hour = gx6605s_read(gdev, GX6605S_RTC_HOUR);
    time->tm_mday = gx6605s_read(gdev, GX6605S_RTC_DAY);
    time->tm_mon  = gx6605s_read(gdev, GX6605S_RTC_MONTH);
    time->tm_year = gx6605s_read(gdev, GX6605S_RTC_YEAR);

    return -ENOERR;
}

static state gx6605s_rtc_settime(struct rtc_device *rtc, struct rtc_time *time)
{
    struct gx6605s_device *gdev = rtc_to_gx6605s(rtc);

    gx6605s_write(gdev, GX6605S_RTC_US, time->tm_us);
    gx6605s_write(gdev, GX6605S_RTC_MS, time->tm_ms);
    gx6605s_write(gdev, GX6605S_RTC_SEC, time->tm_sec);
    gx6605s_write(gdev, GX6605S_RTC_MIN, time->tm_min);
    gx6605s_write(gdev, GX6605S_RTC_HOUR, time->tm_hour);
    gx6605s_write(gdev, GX6605S_RTC_DAY, time->tm_mday);
    gx6605s_write(gdev, GX6605S_RTC_MONTH, time->tm_mon);
    gx6605s_write(gdev, GX6605S_RTC_YEAR, time->tm_year);

    return -ENOERR;
}

static state gx6605s_rtc_getalarm(struct rtc_device *rtc, struct rtc_alarm *alarm)
{
    struct gx6605s_device *gdev = rtc_to_gx6605s(rtc);

    alarm->time.tm_us   = gx6605s_read(gdev, GX6605S_RTC_ALM1_US);
    alarm->time.tm_ms   = gx6605s_read(gdev, GX6605S_RTC_ALM1_MS);
    alarm->time.tm_sec  = gx6605s_read(gdev, GX6605S_RTC_ALM1_SEC);
    alarm->time.tm_min  = gx6605s_read(gdev, GX6605S_RTC_ALM1_MIN);
    alarm->time.tm_hour = gx6605s_read(gdev, GX6605S_RTC_ALM1_HOUR);
    alarm->time.tm_mday = gx6605s_read(gdev, GX6605S_RTC_ALM1_DAY);
    alarm->time.tm_mon  = gx6605s_read(gdev, GX6605S_RTC_ALM1_MONTH);
    alarm->time.tm_year = gx6605s_read(gdev, GX6605S_RTC_ALM1_YEAR);
    alarm->enable = !!(gx6605s_read(gdev, GX6605S_RTC_CTL) & GX6605S_RTC_CTL_ALM1);

    return -ENOERR;
}

static state gx6605s_rtc_setalarm(struct rtc_device *rtc, struct rtc_alarm *alarm)
{
    struct gx6605s_device *gdev = rtc_to_gx6605s(rtc);
    uint32_t val;

    gx6605s_write(gdev, GX6605S_RTC_ALM1_US,    alarm->time.tm_us);
    gx6605s_write(gdev, GX6605S_RTC_ALM1_MS,    alarm->time.tm_ms);
    gx6605s_write(gdev, GX6605S_RTC_ALM1_SEC,   alarm->time.tm_sec);
    gx6605s_write(gdev, GX6605S_RTC_ALM1_MIN,   alarm->time.tm_min);
    gx6605s_write(gdev, GX6605S_RTC_ALM1_HOUR,  alarm->time.tm_hour);
    gx6605s_write(gdev, GX6605S_RTC_ALM1_DAY,   alarm->time.tm_mday);
    gx6605s_write(gdev, GX6605S_RTC_ALM1_MONTH, alarm->time.tm_mon);
    gx6605s_write(gdev, GX6605S_RTC_ALM1_YEAR,  alarm->time.tm_year);

    val = gx6605s_read(gdev, GX6605S_RTC_CTL);
    val &= ~GX6605S_RTC_CTL_ALM1;
    val |= !!alarm->enable << 2;
    gx6605s_write(gdev, GX6605S_RTC_CTL, val);

    return -ENOERR;
}

static struct rtc_ops gx6605s_rtc_ops = {
    .get_time = gx6605s_rtc_gettime,
    .set_time = gx6605s_rtc_settime,
    .get_alarm = gx6605s_rtc_getalarm,
    .set_alarm = gx6605s_rtc_setalarm,
};

static void gx6605s_rtc_hwinit(struct gx6605s_device *gdev)
{
    struct rtc_time time = { };
    struct rtc_alarm alarm = { };
    uint32_t val;

    gx6605s_rtc_settime(&gdev->rtc, &time);
    gx6605s_rtc_setalarm(&gdev->rtc, &alarm);

    val = gx6605s_read(gdev, GX6605S_RTC_CTL);
    val |= GX6605S_RTC_CTL_TIME;
    gx6605s_write(gdev, GX6605S_RTC_CTL, val);
}

static state gx6605s_rtc_probe(struct platform_device *pdev, const void *pdata)
{
    struct gx6605s_device *gdev;
    resource_size_t start, size;

    if (platform_resource_type(pdev, 0) != RESOURCE_MMIO)
        return -ENODEV;

    gdev = dev_kzalloc(&pdev->dev, sizeof(*gdev), GFP_KERNEL);
    if (gdev)
        return -ENOERR;
    platform_set_devdata(pdev, gdev);

    start = platform_resource_start(pdev, 0);
    size = platform_resource_size(pdev, 0);
    gdev->base = dev_ioremap(&pdev->dev, start, size);
    if (!gdev->base)
        return -ENOMEM;

    gx6605s_rtc_hwinit(gdev);

    gdev->rtc.ops = &gx6605s_rtc_ops;
    return rtc_register(&gdev->rtc);
}

static struct dt_device_id gx6605s_rtc_ids[] = {
    { .compatible = "nationalchip,gx6605s-rtc" },
    { },  /* NULL */
};

static struct platform_driver gx6605s_rtc_driver = {
    .driver = {
        .name = DRIVER_NAME,
    },
    .dt_table = gx6605s_rtc_ids,
    .probe = gx6605s_rtc_probe,
};

static state gx6605s_rtc_init(void)
{
    return platform_driver_register(&gx6605s_rtc_driver);
}
driver_initcall(gx6605s_rtc_init);
