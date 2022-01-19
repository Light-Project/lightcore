/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#define DRIVER_NAME "pcspkr"
#define pr_fmt(fmt) DRIVER_NAME ": " fmt

#include <initcall.h>
#include <driver/platform.h>
#include <driver/clocksource/i8253.h>
#include <panic.h>
#include <asm/io.h>

extern spinlock_t i8253_lock;

struct pcspkr_device {
    struct panic_work work;
    resource_size_t base;
};

static __always_inline uint8_t
i8253_in(struct pcspkr_device *sdev, int reg)
{
    return inb(sdev->base + reg);
}

static __always_inline void
i8253_out(struct pcspkr_device *sdev, int reg, uint8_t value)
{
    outb(sdev->base + reg, value);
}

static void pcspkr_panic(const void *data)
{
    /* enable speaker output gate */
    uint8_t val = inb(NMISC_BASE);
    outb(NMISC_BASE, val | NMISC_SPEAKER_EN | NMISC_TIMER2_EN);
}

static void pcspkr_hwinit(struct pcspkr_device *sdev)
{
    /*
     * Counter Select: Counter 2 select
     * Read/Write Select: Read/Write LSB then MSB
     * Counter Mode: Square wave output
     * Binary/BCD Select: Binary countdown is used
     */
    i8253_out(sdev, I8253_MODE, I8253_MODE_SEL_TIMER2 | I8253_MODE_ACCESS_WORD | I8253_MODE_MOD_WAVE);
    i8253_out(sdev, I8253_COUNTER2, I8253_LATCH(1000) & 0xff);
    i8253_out(sdev, I8253_COUNTER2, I8253_LATCH(1000) >> 8);
}

static state pcspkr_probe(struct platform_device *pdev, const void *pdata)
{
    struct pcspkr_device *sdev;
    state ret;

    if (platform_resource_type(pdev, 0) != RESOURCE_PMIO)
        return -ENODEV;

    sdev = platform_kzalloc(pdev, sizeof(*sdev), GFP_KERNEL);
    if (!sdev)
        return -ENOMEM;

    sdev->base = platform_resource_start(pdev, 0);
    sdev->work.work = pcspkr_panic;
    sdev->work.data = sdev;

    platform_set_devdata(pdev, sdev);
    pcspkr_hwinit(sdev);

    ret = panic_work_register(&sdev->work);
    BUG_ON(ret);

    return -ENOERR;
}

static const struct dt_device_id pcspkr_ids[] = {
    { .compatible = "ibm,pcspkr" },
    { }, /* NULL */
};

static struct platform_driver pcspkr_driver = {
    .driver = {
        .name = DRIVER_NAME,
    },
    .dt_table = pcspkr_ids,
    .probe = pcspkr_probe,
};

static state pcspkr_init(void)
{
    return platform_driver_register(&pcspkr_driver);
}
driver_initcall(pcspkr_init);
