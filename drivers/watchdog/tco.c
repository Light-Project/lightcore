/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#define DRIVER_NAME "intel-tco"
#define pr_fmt(fmt) DRIVER_NAME ": " fmt

#include <initcall.h>
#include <driver/platform.h>
#include <driver/watchdog.h>
#include <driver/watchdog/tco.h>
#include <printk.h>
#include <asm/io.h>

struct tco_device {
    struct watchdog_device watchdog;
    unsigned int version;
    resource_size_t tco;
    void *gcs_pmc_base;
    spinlock_t lock;
};

#define wdg_to_tco(wdg) \
    container_of(wdg, struct tco_device, watchdog)

static __always_inline unsigned long
tco_time_to_ticks(struct tco_device *tco, unsigned int secs)
{
    return tco->version == 3 ? secs :
            (secs * 1000) / TCO_TICK_TIME_V1;
}

static __always_inline unsigned int
tco_ticks_to_time(struct tco_device *tco, unsigned long ticks)
{
    return tco->version == 3 ? ticks :
            (ticks * TCO_TICK_TIME_V1) / 1000;
}

static __always_inline uint32_t
tco_no_reboot_bit(struct tco_device *tco)
{
    switch (tco->version) {
        case 5: case 3:
            return 0x00000010;
        case 2:
            return 0x00000020;
        case 4: case 1: default:
            return 0x00000002;
    }
}

static state tco_no_reboot(struct tco_device *tco, bool enable)
{
    uint32_t val, check;

    switch (tco->version) {
        case 1:

            return -ENOERR;
        case 2 ... 5:
            val = readl(tco->gcs_pmc_base);

            if (enable)
                val |= tco_no_reboot_bit(tco);
            else
                val &= ~tco_no_reboot_bit(tco);
            writel(tco->gcs_pmc_base, val);

            check = readl(tco->gcs_pmc_base);
            return check == val ? -ENOERR : -EIO;
        case 6:
            val = inw(tco->tco + TCO_CNT1);

            if (enable)
                val |= 0x0001;
            else
                val &= ~0x0001;
            outw(tco->tco, val);

            check = inw(tco->tco + TCO_CNT1);
            return check == val ? -ENOERR : -EIO;
        default:
            return -ENOERR;
    }
}

static state tco_start(struct watchdog_device *wdev)
{
    struct tco_device *tco = wdg_to_tco(wdev);
    uint16_t val;
    state ret;

    spin_lock(&tco->lock);

    if ((ret = tco_no_reboot(tco, false))) {
        dev_err(wdev->dev, "unable to set no_reboot, "
                "reboot disabled by hardware/BIOS\n");
        goto fail;
    }

    if (tco->version == 1)
        outb(tco->tco + TCO_RLD, 0x01);
    else {
        outw(tco->tco + TCO_RLD, 0x01);
    }

    val = inw(tco->tco + TCO_CNT1);
    val &= ~0x0800;
    outw(tco->tco + TCO_CNT1, val);
    val = inw(tco->tco + TCO_CNT1);
    if (val & 0x0800) {
        dev_err(wdev->dev, "unable to start tco\n");
        ret = -EIO;
    }

fail:
    spin_unlock(&tco->lock);
    return ret;
}

static state tco_stop(struct watchdog_device *wdev)
{
    struct tco_device *tco = wdg_to_tco(wdev);
    uint16_t val;
    state ret = -ENOERR;

    spin_lock(&tco->lock);

    val = inw(tco->tco + TCO_CNT1);
    val |= 0x0800;
    outw(tco->tco + TCO_CNT1, val);
    val = inw(tco->tco + TCO_CNT1);
    if (!(val & 0x0800)) {
        dev_err(wdev->dev, "unable to stop tco\n");
        ret = -EIO;
    }

    spin_unlock(&tco->lock);
    return ret;
}

static state tco_feed(struct watchdog_device *wdev)
{
    struct tco_device *tco = wdg_to_tco(wdev);

    spin_lock(&tco->lock);

    if (tco->version == 1) {
        outw(tco->tco + TCO_STS1, 0x08);
        outb(tco->tco + TCO_RLD, 0x01);
    } else {
        outw(tco->tco + TCO_RLD, 0x01);
    }

    spin_unlock(&tco->lock);
    return -ENOERR;
}

static state tco_timeleft_get(struct watchdog_device *wdev, unsigned int *secs)
{
    struct tco_device *tco = wdg_to_tco(wdev);
    uint16_t timeleft;

    spin_lock(&tco->lock);

    if (tco->version == 1) {
        timeleft = inb(tco->tco + TCO_RLD);
        timeleft &= 0x3f;
        if (!(inb(tco->tco + TCO_STS1) & 0x08))
            timeleft += inb(tco->tco + TCO_TMRV1) & 0x3f;
    } else {
        timeleft = inw(tco->tco + TCO_RLD);
        timeleft &= 0x3fff;
    }

    spin_unlock(&tco->lock);

    *secs = tco_ticks_to_time(tco, timeleft);
    return -ENOERR;
}

static state tco_timeout_set(struct watchdog_device *wdev, unsigned int secs)
{
    struct tco_device *tco = wdg_to_tco(wdev);
    unsigned int ticks;
    uint16_t val, check;
    state ret = -ENOERR;

    ticks = tco_time_to_ticks(tco, secs);
    if (ticks < 0x04)
        return -EINVAL;

    if ((tco->version >= 2 && ticks > 0x3ff) ||
	    (tco->version == 1 && ticks > 0x03f))
        return -EINVAL;

    spin_lock(&tco->lock);

    if (tco->version == 1) {
        val = inb(tco->tco + TCO_TMRV1);
        val &= 0xc0;
        val |= ticks;
        outb(tco->tco + TCO_TMRV1, val);
        check = inb(tco->tco + TCO_TMRV1);

        if ((val & 0x3f) != check) {
            ret = -EIO;
            goto fail;
        }
    } else {
        val = inw(tco->tco + TCO_TMRV2);
        val &= 0xfc00;
        val |= ticks;
        outw(tco->tco + TCO_TMRV2, val);
        check = inw(tco->tco + TCO_TMRV2);

        if ((val & 0x3ff) != check) {
            ret = -EIO;
            goto fail;
        }
    }

fail:
    spin_unlock(&tco->lock);
    return ret;
}

static struct watchdog_ops tco_ops = {
    .start = tco_start,
    .stop = tco_stop,
    .feed = tco_feed,
    .timeleft_get = tco_timeleft_get,
    .timeout_set = tco_timeout_set,
};

static state tco_probe(struct platform_device *pdev, const void *pdata)
{
    struct tco_device *tco;
    resource_size_t base, size;

    if (platform_resource_type(pdev, 0) != RESOURCE_PMIO)
        return -ENODEV;

    tco = dev_kzalloc(&pdev->dev, sizeof(*tco), GFP_KERNEL);
    if (!tco)
        return -ENOMEM;

    tco->tco = platform_resource_start(pdev, TCO_RES_TCO);
    tco->version = (size_t)pdata;
    platform_set_devdata(pdev, tco);

    if (tco->version >= 2) {
        base = platform_resource_start(pdev, TCO_RES_MEM);
        size = platform_resource_size(pdev, TCO_RES_MEM);
        tco->gcs_pmc_base = dev_ioremap(&pdev->dev, base, size);
        if (!tco->gcs_pmc_base)
            return -ENOMEM;
    }

    switch (tco->version) {
        case 4 ... 6:
            outw(tco->tco + TCO_STS1, 0x0008);
            outw(tco->tco + TCO_STS2, 0x0002);
            break;
        case 3:
            outl(tco->tco + TCO_STS1, 0x00020008);
            break;
        case 1 ... 2: default:
            outw(tco->tco + TCO_STS1, 0x0008);
            outw(tco->tco + TCO_STS2, 0x0002);
            outw(tco->tco + TCO_STS2, 0x0004);
    };

    tco->watchdog.dev = &pdev->dev;
    tco->watchdog.ops = &tco_ops;
    return watchdog_register(&tco->watchdog);
}

static struct platform_device_id tco_ids[] = {
    { .name = TCO_MATCH_ID(1), .data = (void *)0x01 },
    { .name = TCO_MATCH_ID(2), .data = (void *)0x02 },
    { .name = TCO_MATCH_ID(3), .data = (void *)0x03 },
    { .name = TCO_MATCH_ID(4), .data = (void *)0x04 },
    { .name = TCO_MATCH_ID(5), .data = (void *)0x05 },
    { .name = TCO_MATCH_ID(6), .data = (void *)0x06 },
    { }, /* NULL */
};

static struct platform_driver tco_driver = {
    .driver = {
        .name = DRIVER_NAME,
    },
    .platform_table = tco_ids,
    .probe = tco_probe,
};

static state tco_init(void)
{
    return platform_driver_register(&tco_driver);
}
driver_initcall(tco_init);
