/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#define DRIVER_NAME "hpet"
#define pr_fmt(fmt) DRIVER_NAME ": " fmt

#include <initcall.h>
#include <irq.h>
#include <gunit.h>
#include <delay.h>
#include <crash.h>
#include <ioops.h>
#include <driver/platform.h>
#include <driver/irqchip.h>
#include <driver/clockevent.h>
#include <driver/clocksource.h>
#include <driver/clocksource/hpet.h>

#define HPET_MIN_CYCLES 128
#define HPET_MIN_DELTA  (HPET_MIN_CYCLES + (HPET_MIN_CYCLES >> 1))

bool hpet_ready;

struct hpet_device {
    struct clocksource_device clksrc;
    struct hpet_channel **channels;
    unsigned int channel_num;
    uint64_t freq;
    void *base;
};

struct hpet_channel {
    struct clockevent_device clkevt;
    struct irqchip_channel *irqchip;
    struct hpet_device *hpet;
    unsigned int index;
};

#define clksrc_to_hpet(cdev) \
    container_of(cdev, struct hpet_device, clksrc)

#define clkevt_to_hpet(cdev) \
    container_of(cdev, struct hpet_channel, clkevt)

static __always_inline uint32_t
hpet_read(struct hpet_device *hdev, int reg)
{
    return readl(hdev->base + reg);
}

static __always_inline void
hpet_write(struct hpet_device *hdev, int reg, uint32_t value)
{
    writel(hdev->base + reg, value);
}

static void hpet_general_start(struct hpet_device *hdev)
{
    uint32_t val;

    val = hpet_read(hdev, HPET_GCONF);
    val |= HPET_GCONF_ENABLE;
    hpet_write(hdev, HPET_GCONF, val);
}

static void hpet_general_stop(struct hpet_device *hdev)
{
    uint32_t val;

    val = hpet_read(hdev, HPET_GCONF);
    val &= ~HPET_GCONF_ENABLE;
    hpet_write(hdev, HPET_GCONF, val);
}

static void hpet_general_reset(struct hpet_device *hdev)
{
    hpet_write(hdev, HPET_MAIN, 0);
    hpet_write(hdev, HPET_MAIN + 4, 0);
}

static void hpet_general_restart(struct hpet_device *hdev)
{
    hpet_general_stop(hdev);
    hpet_general_reset(hdev);
    hpet_general_start(hdev);
}

static void hpet_general_legacy(struct hpet_device *hdev)
{
    uint32_t val;

    val = hpet_read(hdev, HPET_GCONF);
    val |= HPET_GCONF_LEGACY;
    hpet_write(hdev, HPET_GCONF, val);
}

static irqreturn_t hpet_clockevent_handle(irqnr_t vector, void *data)
{
    struct hpet_channel *channel = data;
    channel->clkevt.event_handle(&channel->clkevt);
    return IRQ_RET_HANDLED;
}

static state hpet_next_event(struct clockevent_device *cdev, uint64_t delta)
{
    struct hpet_channel *channel = clkevt_to_hpet(cdev);
    struct hpet_device *hdev = channel->hpet;
    unsigned int index = channel->index;
    uint32_t val;
    int32_t cmp;

    /*
     * HPETs are a complete disaster. The compare register is
     * based on a equal comparison and neither provides a less
     * than or equal functionality (which would require to take
     * the wraparound into account) nor a simple count down event
     * mode. Further the write to the comparator register is
     * delayed internally up to two HPET clock cycles in certain
     * chipsets (ATI, ICH9,10). Some newer AMD chipsets have even
     * longer delays. We worked around that by reading back the
     * compare register, but that required another workaround for
     * ICH9,10 chips where the first readout after write can
     * return the old stale value. We already had a minimum
     * programming delta of 5us enforced, but a NMI or SMI hitting
     * between the counter readout and the comparator write can
     * move us behind that point easily. Now instead of reading
     * the compare register back several times, we make the ETIME
     * decision based on the following: Return ETIME if the
     * counter value after the write is less than HPET_MIN_CYCLES
     * away from the event or if the counter is already ahead of
     * the event. The minimum programming delta for the generic
     * clockevents code is set to 1.5 * HPET_MIN_CYCLES.
     */

    val = hpet_read(hdev, HPET_MAIN);
    val += (uint32_t)delta;
    hpet_write(hdev, HPET_CMP(index), val);

    cmp = (val - hpet_read(hdev, HPET_MAIN));
    return cmp < HPET_MIN_CYCLES ? -ETIME : -ENOERR;
}

static state hpet_state_oneshot(struct clockevent_device *cdev)
{
    struct hpet_channel *channel = clkevt_to_hpet(cdev);
    struct hpet_device *hdev = channel->hpet;
    unsigned int index = channel->index;
    uint32_t val;

    val = hpet_read(hdev, HPET_CFG(index));
    val &= ~HPET_CONF_PERIODIC;
    val |= HPET_CONF_32BIT | HPET_CONF_INT_ENABLE;
    hpet_write(hdev, HPET_CFG(index), val);

    return -ENOERR;
}

static state hpet_state_periodic(struct clockevent_device *cdev, uint64_t delta)
{
    struct hpet_channel *channel = clkevt_to_hpet(cdev);
    struct hpet_device *hdev = channel->hpet;
    unsigned int index = channel->index;
    uint32_t val, cmp;

    hpet_general_stop(hdev);
    cmp = delta;
    cmp += hpet_read(hdev, HPET_MAIN);
    val = hpet_read(hdev, HPET_CFG(index));
    val |= HPET_CONF_32BIT | HPET_CONF_PERIODIC | HPET_CONF_SETVAL | HPET_CONF_INT_ENABLE;
    hpet_write(hdev, HPET_CFG(index), val);
    hpet_write(hdev, HPET_CMP(index), cmp);

    /*
     * HPET on AMD 81xx needs a second write (with HPET_TN_SETVAL
     * cleared) to T0_CMP to set the period. The HPET_TN_SETVAL
     * bit is automatically cleared after the first write.
     * (See AMD-8111 HyperTransport I/O Hub Data Sheet,
     * Publication # 24674)
     */

    udelay(1);
    hpet_write(hdev, HPET_CMP(index), delta);
    hpet_general_start(hdev);

    return -ENOERR;
}

static state hpet_state_shutdown(struct clockevent_device *cdev)
{
    struct hpet_channel *channel = clkevt_to_hpet(cdev);
    struct hpet_device *hdev = channel->hpet;
    unsigned int index = channel->index;
    uint32_t val;

    val = hpet_read(hdev, HPET_CFG(index));
    val &= ~HPET_CONF_INT_ENABLE;
    hpet_write(hdev, HPET_CFG(index), val);

    return -ENOERR;
}

static struct clockevent_ops hpet_clockevent_ops = {
    .next_event = hpet_next_event,
    .state_oneshot = hpet_state_oneshot,
    .state_periodic = hpet_state_periodic,
    .state_shutdown = hpet_state_shutdown,
};

#ifdef CONFIG_CLKSRC_HPET
static uint64_t hpet_main_read(struct clocksource_device *cdev)
{
    struct hpet_device *hdev = clksrc_to_hpet(cdev);
    return readq(hdev->base + HPET_MAIN);
}

static struct clocksource_ops hpet_clocksource_ops = {
    .read = hpet_main_read,
};
#endif

static state hpet_clockevent_init(struct platform_device *pdev)
{
    struct hpet_device *hdev = platform_get_devdata(pdev);
    struct hpet_channel *channel, *channels, **ctable;
    unsigned int count, number;
    uint32_t val, vendor;
    state ret;

    val = hpet_read(hdev, HPET_GCAP);
    vendor = (val & HPET_GCAP_VENDOR) >> 16;
    number = ((val & HPET_GCAP_NUMBER) >> 8) + 1;
    platform_debug(pdev, "version 0x%04x, %u channels\n", vendor, number);

    ctable = platform_kzalloc(pdev, sizeof(*ctable) * number + sizeof(*channel) * number, GFP_KERNEL);
    channels = (void *)ctable + (sizeof(*ctable) * number);
    hdev->channels = ctable;
    if (!ctable)
        return -ENOMEM;

    for (count = 0; count < 1; ++count) {
        channel = &channels[count];
        ctable[count] = channel;

        channel->hpet = hdev;
        channel->index = count;
        channel->clkevt.device = &pdev->dev;
        channel->clkevt.ops = &hpet_clockevent_ops;
        channel->clkevt.rating = 220;
        channel->clkevt.capability = CLKEVT_CAP_ONESHOT;

        channel->irqchip = dt_irqchip_channel(pdev->dt_node, count);
        if (!channel->irqchip) {
            platform_err(pdev, "unable to request irq channel\n");
            return -ENODEV;
        }

        ret = platform_irq_request(pdev, I8253_IRQ, 0, hpet_clockevent_handle, channel, DRIVER_NAME);
        if (ret) {
            platform_err(pdev, "unable to request irq\n");
            return ret;
        }

        val = hpet_read(hdev, HPET_CFG(count));
        if (val & HPET_CONF_PERIODIC_CAP)
            channel->clkevt.capability |= CLKEVT_CAP_PERIODIC;
    }

    for (count = 0; count < 1; ++count) {
        channel = ctable[count];
        ret = clockevent_config_register(&channel->clkevt, hdev->freq, HPET_MIN_DELTA, 0x7fffffff);
        BUG_ON(ret);
        ret = irqchip_pass(channel->irqchip);
        BUG_ON(ret);
    }

    return -ENOERR;
}

static state hpet_probe(struct platform_device *pdev, const void *pdata)
{
    struct hpet_device *hdev;
    char gbuff[GFREQ_BUFF];
    uint32_t val;
    state ret;

    hdev = platform_kzalloc(pdev, sizeof(*hdev), GFP_KERNEL);
    if (!hdev)
        return -ENOMEM;

    hdev->base = platform_resource_ioremap(pdev, 0);
    if (!hdev->base)
        return -ENOMEM;

    val = hpet_read(hdev, HPET_GCAP);
    if (val == 0xffffffff || val == 0x00000000)
        return -ENODEV;

    val = hpet_read(hdev, HPET_PERIOD);
    hdev->freq = FSEC_PER_SEC / val;

    gfreq(gbuff, hdev->freq);
    platform_info(pdev, "main counter tick period %s\n", gbuff);

    platform_set_devdata(pdev, hdev);
    hpet_general_restart(hdev);
    hpet_general_legacy(hdev);

#ifdef CONFIG_CLKSRC_HPET
    hdev->clksrc.device = &pdev->dev;
    hdev->clksrc.rating = CLOCK_RATING_GOOD + 20;
    hdev->clksrc.ops = &hpet_clocksource_ops;
    ret = clocksource_config_register(&hdev->clksrc, hdev->freq, BIT_RANGE(31, 0));
    BUG_ON(ret);
#endif

    ret = hpet_clockevent_init(pdev);
    if (ret) {
        BUG();
        return -ENOERR;
    }

    hpet_ready = true;
    return -ENOERR;
};

static const struct dt_device_id hpet_device_id[] = {
    { .compatible = "intel,hpet" },
    { }, /* NULL */
};

static struct platform_driver hpet_driver = {
    .driver = {
        .name = DRIVER_NAME,
    },
    .dt_table = hpet_device_id,
    .probe = hpet_probe,
};

static state hpet_init(void)
{
    return platform_driver_register(&hpet_driver);
}
clockevent_initcall(hpet_init);
