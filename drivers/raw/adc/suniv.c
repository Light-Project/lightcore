/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#define DRIVER_NAME "suniv-adc"

#include <mm.h>
#include <initcall.h>
#include <driver/platform.h>
#include <driver/raw.h>
#include <driver/raw/suniv.h>
#include <asm/io.h>

struct suniv_adc_device {
    struct raw_channel channel;
    void *mmio;
};

#define channel_to_sdev(chan) \
    container_of(chan, struct suniv_adc_device, channel)

static state suniv_adc_read(struct raw_channel *channel, ssize_t *val)
{
    struct suniv_adc_device *sdev = channel_to_sdev(channel);
    *val = readl(sdev->mmio + SUNIV_ADC_DATA);
}

static struct raw_ops suniv_adc_ops = {
};

static state suniv_adc_probe(struct platform_device *pdev, void *pdata)
{
    struct suniv_adc_device *sdev;

    return -ENOERR;
}

static struct dt_device_id suniv_adc_ids[] = {
    { .compatible = "allwinner,suniv-adc" },
    { }, /* NULL */
};

static struct platform_driver suniv_adc_driver = {
    .driver = {
        .name = DRIVER_NAME,
    },
    .dt_table = suniv_adc_ids,
    .probe = suniv_adc_probe,
};

static state suniv_adc_init(void)
{
    return platform_driver_register(&suniv_adc_driver);
}
driver_initcall(suniv_adc_init);
