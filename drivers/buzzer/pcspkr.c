/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#define DRIVER_NAME "pcspkr"
#define pr_fmt(fmt) DRIVER_NAME ": " fmt

#include <initcall.h>
#include <ioops.h>
#include <driver/buzzer.h>
#include <driver/platform.h>
#include <driver/clocksource/i8253.h>
#include <driver/platform/ibmpc.h>
#include <panic.h>

extern spinlock_t i8253_lock;

struct pcspkr_device {
    struct buzzer_device buzzer;
    resource_size_t base;
    enum buzzer_frequency freq;
};

#define buzzer_to_pcspkr(ptr) \
    container_of(ptr, struct pcspkr_device, buzzer)

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

static state pcspkr_start(struct buzzer_device *bdev)
{
    uint8_t val;

    spin_lock(&i8253_lock);
    val = inb(IBMPC_NMISC_BASE);
    val |= IBMPC_NMISC_SPEAKER_EN | IBMPC_NMISC_TIMER2_EN;
    outb(IBMPC_NMISC_BASE, val);
    spin_unlock(&i8253_lock);

    return -ENOERR;
}

static state pcspkr_stop(struct buzzer_device *bdev)
{
    uint8_t val;

    spin_lock(&i8253_lock);
    val = inb(IBMPC_NMISC_BASE);
    val &= ~(IBMPC_NMISC_SPEAKER_EN | IBMPC_NMISC_TIMER2_EN);
    outb(IBMPC_NMISC_BASE, val);
    spin_unlock(&i8253_lock);

    return -ENOERR;
}

static state pcspkr_freq_get(struct buzzer_device *bdev, enum buzzer_frequency *freq)
{
    struct pcspkr_device *sdev = buzzer_to_pcspkr(bdev);
    *freq = sdev->freq;
    return -ENOERR;
}

static state pcspkr_freq_set(struct buzzer_device *bdev, enum buzzer_frequency freq)
{
    struct pcspkr_device *sdev = buzzer_to_pcspkr(bdev);

    sdev->freq = freq;
    spin_lock(&i8253_lock);

    /*
     * Counter Select: Counter 2 select
     * Read/Write Select: Read/Write LSB then MSB
     * Counter Mode: Square wave output
     * Binary/BCD Select: Binary countdown is used
     */
    i8253_out(sdev, I8253_MODE, I8253_MODE_SEL_TIMER2 | I8253_MODE_ACCESS_WORD | I8253_MODE_MOD_WAVE);
    i8253_out(sdev, I8253_LATCH2, I8253_LATCH(freq) & 0xff);
    i8253_out(sdev, I8253_LATCH2, I8253_LATCH(freq) >> 8);

    spin_unlock(&i8253_lock);
    return -ENOERR;
}

static void pcspkr_panic(struct buzzer_device *bdev)
{
    struct pcspkr_device *sdev = buzzer_to_pcspkr(bdev);
    uint8_t val;

    i8253_out(sdev, I8253_MODE, I8253_MODE_SEL_TIMER2 | I8253_MODE_ACCESS_WORD | I8253_MODE_MOD_WAVE);
    i8253_out(sdev, I8253_LATCH2, I8253_LATCH(1000) & 0xff);
    i8253_out(sdev, I8253_LATCH2, I8253_LATCH(1000) >> 8);

    val = inb(IBMPC_NMISC_BASE);
    val |= IBMPC_NMISC_SPEAKER_EN | IBMPC_NMISC_TIMER2_EN;
    outb(IBMPC_NMISC_BASE, val);
}

static struct buzzer_ops pcspkr_ops = {
    .start = pcspkr_start,
    .stop = pcspkr_stop,
    .freq_get = pcspkr_freq_get,
    .freq_set = pcspkr_freq_set,
    .panic = pcspkr_panic,
};

static state pcspkr_probe(struct platform_device *pdev, const void *pdata)
{
    struct pcspkr_device *sdev;

    if (platform_resource_type(pdev, 0) != RESOURCE_PMIO)
        return -ENODEV;

    sdev = platform_kzalloc(pdev, sizeof(*sdev), GFP_KERNEL);
    if (!sdev)
        return -ENOMEM;

    sdev->base = platform_resource_start(pdev, 0);
    sdev->buzzer.dev = &pdev->dev;
    sdev->buzzer.ops = &pcspkr_ops;
    platform_set_devdata(pdev, sdev);

    return buzzer_register(&sdev->buzzer);
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
