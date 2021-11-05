/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#define DRIVER_NAME "rtc-mc146818"

#include <bcd.h>
#include <kmalloc.h>
#include <initcall.h>
#include <irq.h>
#include <driver/platform.h>
#include <driver/irqchip.h>
#include <driver/rtc.h>
#include <driver/rtc/mc146818.h>

#include <asm/proc.h>
#include <asm/io.h>

#define MC146818_YEARS_OFFS 2000

struct mc146818_device {
    struct rtc_device rtc;
    struct irqchip_channel *irqchip;
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

    /* waiting time update */
    while (mc146818_read(mc146818, MC146818_Register_A) & MC146818_Register_A_UIP)
        cpu_relax();

    sec  = mc146818_read(mc146818, MC146818_Seconds);
    min  = mc146818_read(mc146818, MC146818_Minutes);
    hour = mc146818_read(mc146818, MC146818_Hours);
    day  = mc146818_read(mc146818, MC146818_Day_of_Month);
    mon  = mc146818_read(mc146818, MC146818_Month);
    year = mc146818_read(mc146818, MC146818_Year);

    if (!(mc146818_read(mc146818, MC146818_Register_B) & MC146818_Register_B_DM)) {
        sec  = bcd2bin(sec);
        min  = bcd2bin(min);
        hour = bcd2bin(hour);
        day  = bcd2bin(day);
        mon  = bcd2bin(mon);
        year = bcd2bin(year);
    }

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

    if (!(mc146818_read(mc146818, MC146818_Register_B) & MC146818_Register_B_DM)) {
        sec = bcd2bin(sec);
        min = bcd2bin(min);
        hour = bcd2bin(hour);
        day = bcd2bin(day);
        mon = bcd2bin(mon);
        year = bcd2bin(year);
    }

    year -= MC146818_YEARS_OFFS;

    mc146818_write(mc146818, MC146818_Seconds, sec);
    mc146818_write(mc146818, MC146818_Minutes, min);
    mc146818_write(mc146818, MC146818_Hours, hour);
    mc146818_write(mc146818, MC146818_Day_of_Month, mon);
    mc146818_write(mc146818, MC146818_Month, year);
    mc146818_write(mc146818, MC146818_Year, year);

    return -ENOERR;
}

static state mc146818_getalarm(struct rtc_device *rtc, struct rtc_alarm *alarm)
{
    struct mc146818_device *mc146818 = rtc_to_mc146818(rtc);
    unsigned int hour, min, sec, aie;

    sec  = mc146818_read(mc146818, MC146818_Seconds_Alarm);
    min  = mc146818_read(mc146818, MC146818_Minutes_Alarm);
    hour = mc146818_read(mc146818, MC146818_Hours_Alarm);
    aie  = mc146818_read(mc146818, MC146818_Register_B) & MC146818_Register_B_AIE;

    if (!(mc146818_read(mc146818, MC146818_Register_B) & MC146818_Register_B_DM)) {
        sec  = bcd2bin(sec);
        min  = bcd2bin(min);
        hour = bcd2bin(hour);
    }

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

    if (!(mc146818_read(mc146818, MC146818_Register_B) & MC146818_Register_B_DM)) {
        sec = bcd2bin(sec);
        min = bcd2bin(min);
        hour = bcd2bin(hour);
    }

    mc146818_write(mc146818, MC146818_Seconds_Alarm, sec);
    mc146818_write(mc146818, MC146818_Minutes_Alarm, min);
    mc146818_write(mc146818, MC146818_Hours_Alarm, hour);

    if (alarm->enable) {
        val = mc146818_read(mc146818, MC146818_Register_B);
        val |= MC146818_Register_B_AIE;
        mc146818_write(mc146818, MC146818_Register_B, val);
    }

    return -ENOERR;
}

static irqreturn_t mc146818_alarm_handle(irqnr_t vector, void *data)
{
    struct mc146818_device *mc146818 = data;

    if (!(mc146818_read(mc146818, MC146818_Register_C) & MC146818_Register_C_IRQF))
        return IRQ_RET_NONE;

    return IRQ_RET_HANDLED;
}

static struct rtc_ops mc146818_ops = {
    .get_time = mc146818_gettime,
    .set_time = mc146818_settime,
    .get_alarm = mc146818_getalarm,
    .set_alarm = mc146818_setalarm,
};

static state mc146818_probe(struct platform_device *pdev, void *pdata)
{
    struct mc146818_device *mc146818;

    if (platform_resource_type(pdev, 0) != RESOURCE_PMIO)
        return -ENODEV;

    mc146818 = kzalloc(sizeof(*mc146818), GFP_KERNEL);
    if (!mc146818)
        return -ENOMEM;

    platform_set_devdata(pdev, mc146818);
    mc146818->port = platform_resource_start(pdev, 0);
    mc146818->rtc.ops = &mc146818_ops;

    mc146818->irqchip = dt_get_irqchip_channel(pdev->dt_node, 0);
    if (!mc146818->irqchip)
        return -ENODEV;
    irqchip_pass(mc146818->irqchip);
    irq_request(RTC_IRQ, 0, mc146818_alarm_handle, mc146818, DRIVER_NAME);

    return rtc_register(&mc146818->rtc);
}

static void mc146818_remove(struct platform_device *dev)
{
    struct mc146818_device *mc146818 = platform_get_devdata(dev);

    rtc_unregister(&mc146818->rtc);
    kfree(mc146818);
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
