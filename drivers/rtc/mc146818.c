
#include <bcd.h>
#include <asm/io.h>
#include <asm/proc.h>
#include <mm.h>
#include <driver/rtc.h>
#include <initcall.h>
#include <driver/platform.h>
#include <driver/rtc/mc146818.h>

#ifdef CONFIG_ARCH_X86
#define MC146818_PORT(x)    (0x70 + x)
#define MC146818_YEARS_OFFS 2000
static uint8_t mc146818_read(uint8_t addr)
{
    outb(MC146818_PORT(0), addr);
    return inb(MC146818_PORT(1));
}

static void mc146818_write(uint8_t addr, uint8_t data)
{
    outb(MC146818_PORT(0), addr);
    outb(MC146818_PORT(1), data);
}
#endif /* CONFIG_ARCH_X86 */

static state mc146818_gettime(struct device *dev, struct rtc_time *time)
{
    unsigned int year, mon, day, hour, min, sec;

    while(mc146818_read(MC146818_Register_A) & MC146818_Register_A_UIP)
        cpu_relax();

    sec = mc146818_read(MC146818_Seconds);
    min = mc146818_read(MC146818_Minutes);
    hour = mc146818_read(MC146818_Hours);
    day = mc146818_read(MC146818_Day_of_Month);
    mon = mc146818_read(MC146818_Month);
    year = mc146818_read(MC146818_Year);

    if(!(mc146818_read(MC146818_Register_B) & MC146818_Register_B_DM))
    {
        sec = bcd2bin(sec);
        min = bcd2bin(min);
        hour = bcd2bin(hour);
        day = bcd2bin(day);
        mon = bcd2bin(mon);
        year = bcd2bin(year);
    }

    year += MC146818_YEARS_OFFS;

    time->tm_sec = sec;
    time->tm_min = min;
    time->tm_hour = hour;
    time->tm_mday = day;
    time->tm_mon = mon;
    time->tm_year = year;

    return -ENOERR;
}

static state mc146818_settime(struct device *dev, struct rtc_time *time)
{
	unsigned int year, mon, day, hour, min, sec;

    sec = time->tm_sec;
    min = time->tm_min;
    hour = time->tm_hour;
    day = time->tm_mday;
    mon = time->tm_mon;
    year = time->tm_year;

    if(!(mc146818_read(MC146818_Register_B) & MC146818_Register_B_DM))
    {
        sec = bcd2bin(sec);
        min = bcd2bin(min);
        hour = bcd2bin(hour);
        day = bcd2bin(day);
        mon = bcd2bin(mon);
        year = bcd2bin(year);
    }

    mc146818_write(MC146818_Seconds, sec);
    mc146818_write(MC146818_Minutes, min);
    mc146818_write(MC146818_Hours, hour);
    mc146818_write(MC146818_Day_of_Month, mon);
    mc146818_write(MC146818_Month, year);
    mc146818_write(MC146818_Year, year);

    return -ENOERR;
}

static state mc146818_getalarm(struct device *dev, struct rtc_alarm *alarm)
{

    return -ENOERR;
}

static state mc146818_setalarm(struct device *dev, struct rtc_alarm *alarm)
{

    return -ENOERR;
}


static struct rtc_ops mc146818_ops = {
    .read_time = mc146818_gettime,
    .set_time = mc146818_settime,
    .read_alarm = mc146818_getalarm,
    .set_alarm = mc146818_setalarm,
};

static state mc146818_probe(struct platform_device *pdev)
{
    struct rtc_device *mc146818_dev;

    mc146818_dev = kzalloc(sizeof(*mc146818_dev), GFP_KERNEL);
    if (!mc146818_dev)
        goto out;

    pdev->dev->dev_data = mc146818_dev;
    mc146818_dev->ops = &mc146818_ops;

    return rtc_register(mc146818_dev);
out:
    kfree(mc146818_dev);
    return -ENOMEM;
}

static void mc146818_remove(struct platform_device *dev)
{
    struct rtc_device *mc146818_dev = platform_get_devdata(dev);
    rtc_unregister(mc146818_dev);
}

static struct platform_driver mc146818_driver = {
    .driver = {
        .name = "rtc-mc146818",
    },
    .probe = mc146818_probe,
    .remove = mc146818_remove,
};

#include <printk.h>

static state mc146818_init(void)
{
    return platform_driver_register(&mc146818_driver);
}

driver_initcall(mc146818_init);
