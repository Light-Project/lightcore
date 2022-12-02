/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#define DRIVER_NAME "i8253"

#include <initcall.h>
#include <irq.h>
#include <ioops.h>
#include <crash.h>
#include <export.h>
#include <timekeeping.h>
#include <driver/platform.h>
#include <driver/irqchip.h>
#include <driver/clockevent.h>
#include <driver/clocksource.h>
#include <driver/clocksource/i8253.h>

/*
 * Protects access to I/O ports
 * 0x40-0x43: i8253 / i8254
 * 0x61-0x61: NMI Control Register
 */
SPIN_LOCK(i8253_lock);
EXPORT_SYMBOL(i8253_lock);

#ifdef CONFIG_CLKEVT_HPET
extern bool hpet_ready;
#endif

struct i8253_device {
    struct clockevent_device clkevt;
    struct clocksource_device clksrc;
    struct irqchip_channel *irqchip;
    resource_size_t base;
};

#define clkevt_to_i8253(cdev) \
    container_of(cdev, struct i8253_device, clkevt)

#define clksrc_to_i8253(cdev) \
    container_of(cdev, struct i8253_device, clksrc)

static __always_inline uint8_t
i8253_in(struct i8253_device *idev, int reg)
{
    return inb_p(idev->base + reg);
}

static __always_inline void
i8253_out(struct i8253_device *idev, int reg, uint8_t value)
{
    outb_p(idev->base + reg, value);
}

static irqreturn_t i8253_clockevent_handle(irqnr_t vector, void *data)
{
    struct i8253_device *idev = data;
    idev->clkevt.event_handle(&idev->clkevt);
    return IRQ_RET_HANDLED;
}

static state i8253_next_event(struct clockevent_device *cdev, uint64_t delta)
{
    struct i8253_device *idev = clkevt_to_i8253(cdev);

    spin_lock(&i8253_lock);

    i8253_out(idev, I8253_LATCH0, delta & 0xff);
    i8253_out(idev, I8253_LATCH0, delta >> 8);

    spin_unlock(&i8253_lock);
    return -ENOERR;
}

static state i8253_state_oneshot(struct clockevent_device *cdev)
{
    struct i8253_device *idev = clkevt_to_i8253(cdev);

    spin_lock(&i8253_lock);

    /*
     * Counter Select: Counter 0 select
     * Read/Write Select: Read/Write LSB then MSB
     * Counter Mode: Software triggered strobe
     * Binary/BCD Select: Binary countdown is used
     */
    i8253_out(idev, I8253_MODE, I8253_MODE_ACCESS_WORD | I8253_MODE_MOD_STRIG);

    spin_unlock(&i8253_lock);
    return -ENOERR;
}

static state i8253_state_periodic(struct clockevent_device *cdev, uint64_t delta)
{
    struct i8253_device *idev = clkevt_to_i8253(cdev);

    spin_lock(&i8253_lock);

    /*
     * Counter Select: Counter 0 select
     * Read/Write Select: Read/Write LSB then MSB
     * Counter Mode: Rate generator (divide by n counter)
     * Binary/BCD Select: Binary countdown is used
     */
    i8253_out(idev, I8253_MODE, I8253_MODE_ACCESS_WORD | I8253_MODE_MOD_RATE);
    i8253_out(idev, I8253_LATCH0, delta & 0xff);
    i8253_out(idev, I8253_LATCH0, delta >> 8);

    spin_unlock(&i8253_lock);
    return -ENOERR;
}

static state i8253_state_shutdown(struct clockevent_device *cdev)
{
    struct i8253_device *idev = clkevt_to_i8253(cdev);

    spin_lock(&i8253_lock);

    i8253_out(idev, I8253_MODE, I8253_MODE_ACCESS_WORD);
    i8253_out(idev, I8253_LATCH0, 0);
    i8253_out(idev, I8253_LATCH0, 0);

    spin_unlock(&i8253_lock);
    return -ENOERR;
}

static struct clockevent_ops i8253_ops = {
    .next_event = i8253_next_event,
    .state_oneshot = i8253_state_oneshot,
    .state_periodic = i8253_state_periodic,
    .state_shutdown = i8253_state_shutdown,
};

#ifdef CONFIG_CLKSRC_I8253
static uint64_t i8253_read(struct clocksource_device *cdev)
{
    struct i8253_device *idev = clksrc_to_i8253(cdev);
    static uint64_t old_tick;
    uint64_t tick;
    static uint16_t old_count;
    uint16_t count, latch;

    spin_lock(&i8253_lock);
    tick = ticktime;

    /* latched the count then read count */
    i8253_out(idev, I8253_MODE, 0x00);
    count = i8253_in(idev, I8253_COUNTER0);
    count |= i8253_in(idev, I8253_COUNTER0) << 8;

    /*
     * Maybe i8253 clockevent not used,
     * so it is not initialized.
     */
    if (count > (latch = I8253_LATCH(CONFIG_SYSTICK_FREQ))) {
        i8253_out(idev, I8253_MODE, I8253_MODE_ACCESS_WORD | I8253_MODE_MOD_RATE);
        i8253_out(idev, I8253_LATCH0, latch & 0xff);
        i8253_out(idev, I8253_LATCH0, latch >> 8);
        count = latch - 1;
    }

    /*
     * It's possible for count to appear to go the wrong way for a
     * couple of reasons:
     *
     *  1. The timer counter underflows, but we haven't handled the
     *     resulting interrupt and incremented jiffies yet.
     *  2. Hardware problem with the timer, not giving us continuous time,
     *     the counter does small "jumps" upwards on some Pentium systems,
     *     (see c't 95/10 page 335 for Neptun bug.)
     *
     * Previous attempts to handle these cases intelligently were
     * buggy, so we just do the simple thing now.
     */
    if (count > old_count && tick == old_tick)
        count = old_count;

    old_count = count;
    old_tick = tick;

    spin_unlock(&i8253_lock);

    /* reversal count */
    count = (latch - 1) - count;
    return (ticktime * latch) + count;
}

static struct clocksource_ops i8253_clocksource_ops = {
    .read = i8253_read,
};
#endif

static state i8253_probe(struct platform_device *pdev, const void *pdata)
{
    struct i8253_device *idev;
    state ret;

    idev = platform_kzalloc(pdev, sizeof(*idev), GFP_KERNEL);
    if (!idev)
        return -ENOMEM;

    idev->base = platform_resource_start(pdev, 0);
    platform_set_devdata(pdev, idev);

#ifdef CONFIG_CLKEVT_HPET
    if (hpet_ready)
        goto skip_clkevt;
#endif

    idev->irqchip = dt_irqchip_channel(pdev->dt_node, 0);
    if (!idev->irqchip) {
        platform_err(pdev, "unable to request irq channel\n");
        return -ENODEV;
    }

    ret = platform_irq_request(pdev, I8253_IRQ, 0, i8253_clockevent_handle, idev, DRIVER_NAME);
    if (ret) {
        platform_err(pdev, "unable to request irq\n");
        return ret;
    }

    idev->clkevt.device = &pdev->dev;
    idev->clkevt.ops = &i8253_ops;
    idev->clkevt.rating = 110;
    idev->clkevt.capability = CLKEVT_CAP_PERIODIC | CLKEVT_CAP_ONESHOT;
    ret = clockevent_config_register(&idev->clkevt, I8253_CLKRATE, 0x0f, 0x7fff);
    BUG_ON(ret);

    ret = irqchip_pass(idev->irqchip);
    BUG_ON(ret);

#ifdef CONFIG_CLKEVT_HPET
skip_clkevt:
#endif

#ifdef CONFIG_CLKSRC_I8253
    idev->clksrc.device = &pdev->dev;
    idev->clksrc.rating = 110;
    idev->clksrc.ops = &i8253_clocksource_ops;
    ret = clocksource_config_register(&idev->clksrc, I8253_CLKRATE, BIT_RANGE(31, 0));
    BUG_ON(ret);
#endif

    return -ENOERR;
}

static const struct dt_device_id i8253_device_id[] = {
    { .compatible = "intel,i8253" },
    { }, /* NULL */
};

static struct platform_driver i8253_driver = {
    .driver = {
        .name = DRIVER_NAME,
    },
    .dt_table = i8253_device_id,
    .probe = i8253_probe,
};

static state i8253_init(void)
{
    return platform_driver_register(&i8253_driver);
}
clockevent_initcall(i8253_init);
