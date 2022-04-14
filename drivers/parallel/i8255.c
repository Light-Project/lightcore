/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#define DRIVER_NAME "i8255"
#define pr_fmt(fmt) DRIVER_NAME ": " fmt

#include <initcall.h>
#include <driver/platform.h>
#include <driver/parallel.h>
#include <driver/parallel/i8255.h>
#include <asm/proc.h>
#include <delay.h>
#include <asm/io.h>

#define I8255_TIMEOUT   500

struct i8255_device {
    struct parallel_host parallel;
    resource_size_t base;
};

#define parallel_to_idev(pdev) \
    container_of(pdev, struct i8255_device, parallel)

static __always_inline uint8_t
i8255_in(struct i8255_device *idev, int reg)
{
    return inb(idev->base + reg);
}

static __always_inline void
i8255_out(struct i8255_device *idev, int reg, uint8_t val)
{
    outb(idev->base + reg, val);
}

static bool i8042_wait(struct i8255_device *idev, uint8_t mask, uint8_t pass)
{
    int timeout = I8255_TIMEOUT;
    uint8_t val;

    while (--timeout) {
        val = i8255_in(idev, I8255_STATUS);
        if ((val & mask) == pass)
            break;
        udelay(10);
    }

    return !!timeout;
}

static inline bool i8255_wait_busy(struct i8255_device *idev)
{
    return i8042_wait(idev, I8255_STATUS_BUSY, 0);
}

static state i8255_data_read(struct parallel_host *host, uint8_t *data)
{
    struct i8255_device *idev = parallel_to_idev(host);

    if (!i8255_wait_busy(idev))
        return -EBUSY;

    *data = i8255_in(idev, I8255_DATA);
    return -ENOERR;
}

static state i8255_data_write(struct parallel_host *host, uint8_t data)
{
    struct i8255_device *idev = parallel_to_idev(host);

    if (!i8255_wait_busy(idev))
        return -EBUSY;

    i8255_out(idev, I8255_DATA, data);
    return -ENOERR;
}

static struct parallel_ops i8255_ops = {
    .data_read = i8255_data_read,
    .data_write = i8255_data_write,
};

static bool i8255_detect(struct i8255_device *idev)
{
    /* Reset Port */
    i8255_out(idev, I8255_CTRL, 0);

    if (!i8255_wait_busy(idev))
        return false;
    i8255_out(idev, I8255_DATA, 0x55);

    if (!i8255_wait_busy(idev) ||
        i8255_in(idev, I8255_DATA) != 0x55)
        return false;

    if (!i8255_wait_busy(idev))
        return false;
    i8255_out(idev, I8255_DATA, 0xaa);

    if (!i8255_wait_busy(idev) ||
        i8255_in(idev, I8255_DATA) != 0xaa)
        return false;

    i8255_out(idev, I8255_DATA, 0x00);
    return true;
}

static state i8255_probe(struct platform_device *pdev, const void *pdata)
{
    struct i8255_device *idev;

    if (platform_resource_type(pdev, 0) != RESOURCE_PMIO)
        return -ENXIO;

    idev = dev_kzalloc(&pdev->dev, sizeof(*idev), GFP_KERNEL);
    if (!idev)
        return -ENOMEM;

    idev->base = platform_resource_start(pdev, 0);
    idev->parallel.dev = &pdev->dev;
    idev->parallel.ops = &i8255_ops;
    idev->parallel.dt_node = pdev->dt_node;
    platform_set_devdata(pdev, idev);

    if (!i8255_detect(idev))
        return -ENODEV;

    platform_debug(pdev, "detected port\n");
    parallel_host_register(&idev->parallel);
    return -ENOERR;
}

static struct dt_device_id i8255_ids[] = {
    { .compatible = "intel,i8255" },
    { }, /* NULL */
};

static struct platform_driver i8255_driver = {
    .driver = {
        .name = DRIVER_NAME,
    },
    .dt_table = i8255_ids,
    .probe = i8255_probe,
};

static state i8255_init(void)
{
    return platform_driver_register(&i8255_driver);
}
driver_initcall(i8255_init);
