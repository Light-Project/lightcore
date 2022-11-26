/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#define DRIVER_NAME "ich-gpio"
#define pr_fmt(fmt) DRIVER_NAME ": " fmt

#include <initcall.h>
#include <ioops.h>
#include <driver/platform.h>
#include <driver/gpio.h>
#include <driver/gpio/ich.h>

struct intel_device {
    struct gpio_device gpio;
    resource_size_t base;
    spinlock_t lock;
};

#define gpio_to_intel(ptr) \
    container_of(ptr, struct intel_device, gpio)

static __always_inline uint32_t
intel_read(struct intel_device *idev, unsigned int reg)
{
    return inl(idev->base + reg);
}

static __always_inline void
intel_write(struct intel_device *idev, unsigned int reg, uint32_t value)
{
    return outl(idev->base + reg, value);
}

static state intel_value_get(struct gpio_device *gdev, unsigned int port, bool *value)
{
    struct intel_device *idev = gpio_to_intel(gdev);
    unsigned int index = port / ICH_GPIO_INDEX_SIZE;
    uint32_t regval;

    regval = intel_read(idev, ICH_GPIO_GP_SEL(index));
    *value = !!(regval & BIT(port % ICH_GPIO_INDEX_SIZE));

    return -ENOERR;
}

static state intel_value_set(struct gpio_device *gdev, unsigned int port, bool value)
{
    struct intel_device *idev = gpio_to_intel(gdev);
    unsigned int index = port / ICH_GPIO_INDEX_SIZE;
    uint32_t regval, comp;
    irqflags_t irqflags;

    spin_lock_irqsave(&idev->lock, &irqflags);
    regval = intel_read(idev, ICH_GPIO_GP_SEL(index));
    regval = value
            ? regval | BIT(port % ICH_GPIO_INDEX_SIZE)
            : regval & ~BIT(port % ICH_GPIO_INDEX_SIZE);
    intel_write(idev, ICH_GPIO_GP_SEL(index), regval);
    comp = intel_read(idev, ICH_GPIO_GP_SEL(index));
    spin_unlock_irqrestore(&idev->lock, &irqflags);

    return regval != comp ? -EIO : -ENOERR;
}

static state intel_direction_get(struct gpio_device *gdev, unsigned int port, enum gpio_direction *dir)
{
    struct intel_device *idev = gpio_to_intel(gdev);
    unsigned int index = port / ICH_GPIO_INDEX_SIZE;
    uint32_t regval;

    regval = intel_read(idev, ICH_GPIO_IO_SEL(index));
    if (!(regval & BIT(port % ICH_GPIO_INDEX_SIZE)))
        *dir = GPIO_DIRECTION_INPUT;
    else
        *dir = GPIO_DIRECTION_PUSH;

    return -ENOERR;
}

static state intel_direction_set(struct gpio_device *gdev, unsigned int port, enum gpio_direction dir)
{
    struct intel_device *idev = gpio_to_intel(gdev);
    unsigned int index = port / ICH_GPIO_INDEX_SIZE;
    uint32_t regval, comp;
    irqflags_t irqflags;

    switch (dir) {
        case GPIO_DIRECTION_INPUT:
            spin_lock_irqsave(&idev->lock, &irqflags);
            regval = intel_read(idev, ICH_GPIO_GP_SEL(index));
            regval |= BIT(port % ICH_GPIO_INDEX_SIZE);
            intel_write(idev, ICH_GPIO_GP_SEL(index), regval);
            comp = intel_read(idev, ICH_GPIO_GP_SEL(index));
            spin_unlock_irqrestore(&idev->lock, &irqflags);
            break;

        case GPIO_DIRECTION_PUSH:
            spin_lock_irqsave(&idev->lock, &irqflags);
            regval = intel_read(idev, ICH_GPIO_GP_SEL(index));
            regval &= ~BIT(port % ICH_GPIO_INDEX_SIZE);
            intel_write(idev, ICH_GPIO_GP_SEL(index), regval);
            comp = intel_read(idev, ICH_GPIO_GP_SEL(index));
            spin_unlock_irqrestore(&idev->lock, &irqflags);
            break;

        default:
            return -EOPNOTSUPP;
    }

    return regval != comp ? -EIO : -ENOERR;
}

static struct gpio_ops intel_gpio_ops = {
    .value_get = intel_value_get,
    .value_set = intel_value_set,
    .direction_get = intel_direction_get,
    .direction_set = intel_direction_set,
};

static state ich_gpio_probe(struct platform_device *pdev, const void *pdata)
{
    struct intel_device *idev;

    idev = platform_kzalloc(pdev, sizeof(*idev), GFP_KERNEL);
    if (!idev)
        return -ENOMEM;

    idev->base = platform_resource_start(pdev, 0);
    idev->gpio.dev = &pdev->dev;
    idev->gpio.ops = &intel_gpio_ops;
    idev->gpio.gpio_base = UINT_MAX;
    idev->gpio.gpio_num = BITS_PER_U32 * ICH_GPIO_INDEX_NUM;
    platform_set_devdata(pdev, idev);

    return gpio_register(&idev->gpio);
}

static const struct platform_device_id ich_gpio_ids[] = {
    { .name = INTEL_GPIO_I3100 },
    { .name = INTEL_GPIO_V5 },
    { .name = INTEL_GPIO_V6 },
    { .name = INTEL_GPIO_V7 },
    { .name = INTEL_GPIO_V9 },
    { .name = INTEL_GPIO_V10CP },
    { .name = INTEL_GPIO_V10CS },
    { }, /* NULL */
};

static struct platform_driver ich_gpio_driver = {
    .driver = {
        .name = DRIVER_NAME,
    },
    .platform_table = ich_gpio_ids,
    .probe = ich_gpio_probe,
};

static state ich_gpio_init(void)
{
    return platform_driver_register(&ich_gpio_driver);
}
driver_initcall(ich_gpio_init);
