/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#define DRIVER_NAME "suniv-codec"

#include <initcall.h>
#include <driver/platform.h>
#include <driver/sound.h>

static state suniv_codec_probe(struct platform_device *pdev, const void *pdata)
{

    return -ENOERR;
}

static void suniv_codec_remove(struct platform_device *pdev)
{
}

static struct dt_device_id suniv_codec_ids[] = {
    { .compatible = "allwinner,suniv-codec" },
    { }, /* NULL */
};

static struct platform_driver suniv_codec_driver = {
    .driver = {
        .name = DRIVER_NAME,
    },
    .dt_table = suniv_codec_ids,
    .probe = suniv_codec_probe,
    .remove = suniv_codec_remove,
};

static state suniv_codec_init(void)
{
    return platform_driver_register(&bochs_driver);
}
driver_initcall(suniv_codec_init);
