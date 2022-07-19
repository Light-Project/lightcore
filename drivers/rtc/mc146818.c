/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#define DRIVER_NAME "rtc-mc146818"

#include <bcd.h>
#include <kmalloc.h>
#include <initcall.h>
#include <irq.h>
#include <delay.h>
#include <ioops.h>
#include <driver/platform.h>
#include <driver/irqchip.h>
#include <driver/rtc.h>
#include <driver/rtc/mc146818.h>

#define MC146818_YEARS_OFFS 2000
#define MC146818_TIMEOUT    500

struct mc146818_device {
    struct rtc_device rtc;
    struct irqchip_channel *irqchip;
    spinlock_t lock;
    resource_size_t port;
};

#define rtc_to_mc146818(rdev) \
    container_of(rdev, struct mc146818_device, rtc)

static __always_inline uint8_t
mc146818_read(struct mc146818_device *mc146818, uint8_t addr)
{
    outb(mc146818->port, addr);
    return inb(mc146818->port + 1);
}

static __always_inline void
mc146818_write(struct mc146818_device *mc146818, uint8_t addr, uint8_t data)
{
    outb(mc146818->port, addr);
    outb(mc146818->port + 1, data);
}

static state mc146818_gettime(struct rtc_device *rtc, struct rtc_time *time)
{
    struct mc146818_device *mc146818 = rtc_to_mc146818(rtc);
    unsigned int year, mon, day, hour, min, sec;
    unsigned int timeout = MC146818_TIMEOUT;

    spin_lock(&mc146818->lock);

    /* waiting time update */
    while (mc146818_read(mc146818, MC146818_REGISTER_A) & MC146818_REGISTER_A_UIP && --timeout)
        msleep(1);

    if (!timeout) {
        spin_unlock(&mc146818->lock);
        return -ETIME;
    }

    sec  = mc146818_read(mc146818, MC146818_SECONDS);
    min  = mc146818_read(mc146818, MC146818_MINUTES);
    hour = mc146818_read(mc146818, MC146818_HOURS);
    day  = mc146818_read(mc146818, MC146818_DAY_OF_MONTH);
    mon  = mc146818_read(mc146818, MC146818_MONTH);
    year = mc146818_read(mc146818, MC146818_YEAR);

    if (!(mc146818_read(mc146818, MC146818_REGISTER_B) & MC146818_REGISTER_B_DM)) {
        sec  = bcd2bin(sec);
        min  = bcd2bin(min);
        hour = bcd2bin(hour);
        day  = bcd2bin(day);
        mon  = bcd2bin(mon);
        year = bcd2bin(year);
    }

    spin_unlock(&mc146818->lock);

    year += MC146818_YEARS_OFFS;

    time->tm_sec  = sec;
    time->tm_min  = min;
    time->tm_hour = hour;
    time->tm_mday = day;
    time->tm_mon  = mon;
    time->tm_year = year;

    return -ENOERR;
}

static state mc146818_settime(struct rtc_device *rtc, struct rtc_time *time)
{
    struct mc146818_device *mc146818 = rtc_to_mc146818(rtc);
    unsigned int year, mon, day, hour, min, sec;

    sec  = time->tm_sec;
    min  = time->tm_min;
    hour = time->tm_hour;
    day  = time->tm_mday;
    mon  = time->tm_mon;
    year = time->tm_year;

    year -= MC146818_YEARS_OFFS;

    spin_lock(&mc146818->lock);

    if (!(mc146818_read(mc146818, MC146818_REGISTER_B) & MC146818_REGISTER_B_DM)) {
        sec = bin2bcd(sec);
        min = bin2bcd(min);
        hour = bin2bcd(hour);
        day = bin2bcd(day);
        mon = bin2bcd(mon);
        year = bin2bcd(year);
    }

    mc146818_write(mc146818, MC146818_SECONDS, sec);
    mc146818_write(mc146818, MC146818_MINUTES, min);
    mc146818_write(mc146818, MC146818_HOURS, hour);
    mc146818_write(mc146818, MC146818_DAY_OF_MONTH, mon);
    mc146818_write(mc146818, MC146818_MONTH, year);
    mc146818_write(mc146818, MC146818_YEAR, year);

    spin_unlock(&mc146818->lock);

    return -ENOERR;
}

static state mc146818_getalarm(struct rtc_device *rtc, struct rtc_alarm *alarm)
{
    struct mc146818_device *mc146818 = rtc_to_mc146818(rtc);
    unsigned int hour, min, sec, aie;

    spin_lock(&mc146818->lock);

    sec  = mc146818_read(mc146818, MC146818_SECONDS_ALARM);
    min  = mc146818_read(mc146818, MC146818_MINUTES_ALARM);
    hour = mc146818_read(mc146818, MC146818_HOURS_ALARM);
    aie  = mc146818_read(mc146818, MC146818_REGISTER_B) & MC146818_REGISTER_B_AIE;

    if (!(mc146818_read(mc146818, MC146818_REGISTER_B) & MC146818_REGISTER_B_DM)) {
        sec  = bcd2bin(sec);
        min  = bcd2bin(min);
        hour = bcd2bin(hour);
    }

    spin_unlock(&mc146818->lock);

    alarm->time.tm_sec  = sec;
    alarm->time.tm_min  = min;
    alarm->time.tm_hour = hour;
    alarm->enable = !!aie;

    return -ENOERR;
}

static state mc146818_setalarm(struct rtc_device *rtc, struct rtc_alarm *alarm)
{
    struct mc146818_device *mc146818 = rtc_to_mc146818(rtc);
    unsigned int hour, min, sec;
    uint8_t val;

    sec  = alarm->time.tm_sec;
    min  = alarm->time.tm_min;
    hour = alarm->time.tm_hour;

    spin_lock(&mc146818->lock);

    if (!(mc146818_read(mc146818, MC146818_REGISTER_B) & MC146818_REGISTER_B_DM)) {
        sec = bcd2bin(sec);
        min = bcd2bin(min);
        hour = bcd2bin(hour);
    }

    mc146818_write(mc146818, MC146818_SECONDS_ALARM, sec);
    mc146818_write(mc146818, MC146818_MINUTES_ALARM, min);
    mc146818_write(mc146818, MC146818_HOURS_ALARM, hour);

    if (alarm->enable) {
        val = mc146818_read(mc146818, MC146818_REGISTER_B);
        val |= MC146818_REGISTER_B_AIE;
        mc146818_write(mc146818, MC146818_REGISTER_B, val);
    }

    spin_unlock(&mc146818->lock);

    return -ENOERR;
}

static struct rtc_ops mc146818_ops = {
    .time_get = mc146818_gettime,
    .time_set = mc146818_settime,
    .alarm_get = mc146818_getalarm,
    .alarm_set = mc146818_setalarm,
};

static irqreturn_t mc146818_alarm_handle(irqnr_t vector, void *data)
{
    struct mc146818_device *mc146818 = data;

    if (!(mc146818_read(mc146818, MC146818_REGISTER_C) & MC146818_REGISTER_C_IRQF))
        return IRQ_RET_NONE;

    return IRQ_RET_HANDLED;
}

static void mc146818_hw_init(struct mc146818_device *mc146818)
{
    uint8_t val;

    val = mc146818_read(mc146818, MC146818_REGISTER_B);
    val |= MC146818_REGISTER_B_AIE;
    mc146818_write(mc146818, MC146818_REGISTER_B, val);
}

static state mc146818_probe(struct platform_device *pdev, const void *pdata)
{
    struct mc146818_device *mc146818;
    state ret;

    if (platform_resource_type(pdev, 0) != RESOURCE_PMIO)
        return -EINVAL;

    mc146818 = platform_kzalloc(pdev, sizeof(*mc146818), GFP_KERNEL);
    if (!mc146818)
        return -ENOMEM;

    mc146818->irqchip = dt_irqchip_channel(pdev->dt_node, 0);
    if (!mc146818->irqchip)
        return -ENODEV;

    mc146818->port = platform_resource_start(pdev, 0);
    mc146818->port = platform_resource_start(pdev, 0);
    mc146818->rtc.dev = &pdev->dev;
    mc146818->rtc.ops = &mc146818_ops;
    platform_set_devdata(pdev, mc146818);

    ret = irq_request(RTC_IRQ, 0, mc146818_alarm_handle, mc146818, DRIVER_NAME);
    if (ret) {
        platform_err(pdev, "unable to request irq");
        return ret;
    }

    ret = irqchip_pass(mc146818->irqchip);
    if (ret) {
        platform_err(pdev, "unable to pass irqchip");
        return ret;
    }

    mc146818_hw_init(mc146818);
    return rtc_register(&mc146818->rtc);
}

static void mc146818_remove(struct platform_device *dev)
{
    struct mc146818_device *mc146818 = platform_get_devdata(dev);
    rtc_unregister(&mc146818->rtc);
}

static struct dt_device_id mc146818_ids[] = {
    {.compatible = "motorola,mc146818"},
    {.compatible = "maxim,ds12887"},
    { }, /* NULL */
};

static struct platform_driver mc146818_driver = {
    .driver = {
        .name = DRIVER_NAME,
    },
    .dt_table = mc146818_ids,
    .probe = mc146818_probe,
    .remove = mc146818_remove,
};

static state mc146818_init(void)
{
    return platform_driver_register(&mc146818_driver);
}
driver_initcall(mc146818_init);
