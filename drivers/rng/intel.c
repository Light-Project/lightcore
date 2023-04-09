/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 John Sanpe <sanpeqf@gmail.com>
 */

#define DRIVER_NAME ""
#define pr_fmt(fmt) DRIVER_NAME ": " fmt

#include <initcall.h>
#include <kmalloc.h>
#include <ioops.h>
#include <driver/platform.h>
#include <driver/pci.h>
#include <driver/rng.h>
#include <driver/rng/intel.h>

struct intel_device {
    struct rng_device rng;
    void *base;
};

#define rng_to_idev(rdev) \
    container_of(rdev, struct intel_device, rng)

static __always_inline uint8_t
intel_read(struct intel_device *idev, unsigned int reg)
{
    return readb(idev->base + reg);
}

static __always_inline uint8_t
intel_write(struct intel_device *idev, unsigned int reg, uint8_t val)
{
    writeb(idev->base + reg, val);
    return intel_read(idev, reg);
}

static state intel_rng_enable(struct rng_device *rdev)
{
    struct intel_device *idev = rng_to_idev(rdev);
    uint8_t val;

    val = intel_read(idev, INTEL_RNG_HW);
    if (!(val & INTEL_RNG_HW_ENABLE)) {
        val |= INTEL_RNG_HW_ENABLE;
        val = intel_write(idev, INTEL_RNG_HW, val);

        if (!(val & INTEL_RNG_HW_ENABLE)) {
            dev_err(rdev->dev, "failed to enable rng\n");
            return -EIO;
        }
    }

    return -ENOERR;
}

static state intel_rng_disable(struct rng_device *rdev)
{
    struct intel_device *idev = rng_to_idev(rdev);
    uint8_t val;

    val = intel_read(idev, INTEL_RNG_HW);
    if (val & INTEL_RNG_HW_ENABLE) {
        val &= ~INTEL_RNG_HW_ENABLE;
        val = intel_write(idev, INTEL_RNG_HW, val);
    }

    return -ENOERR;
}

static state intel_rng_read(struct rng_device *rdev, unsigned long *data)
{
    struct intel_device *idev = rng_to_idev(rdev);
    unsigned long val = 0;
    unsigned int count;

    for (count = 0; count < BITS_PER_LONG; count += 8)
        val |= readb(idev->base + INTEL_RNG_DATA) << count;

    *data = val;
    return -ENOERR;
}

static struct rng_ops intel_rng_ops = {
    .enable = intel_rng_enable,
    .disable = intel_rng_disable,
    .read = intel_rng_read,
};

static state intel_rng_probe(struct platform_device *pdev, const void *pdata)
{
    struct intel_device *idev;
    resource_size_t start, size;

    if (platform_resource_type(pdev, 0) != RESOURCE_MMIO)
        return -EINVAL;

    idev = platform_kzalloc(pdev, sizeof(*idev), GFP_KERNEL);
    if (!idev)
        return -ENOMEM;

    start = platform_resource_start(pdev, 0);
    size = platform_resource_size(pdev, 0);
    idev->base = platform_ioremap(pdev, start, size);
    if (!idev->base)
        return -ENOMEM;

    idev->rng.dev = &pdev->dev;
    idev->rng.ops = &intel_rng_ops;
    idev->rng.rating = DEVICE_RATING_GOOD;

    return rng_register(&idev->rng);
}

static const struct platform_device_id intel_rng_match[] = {
    { .name = "intel,rng" },
    { }, /* NULL */
};

static struct platform_driver intel_rng_driver = {
    .driver = {
        .name = DRIVER_NAME,
    },
    .platform_table = intel_rng_match,
    .probe = intel_rng_probe,
};

static const struct pci_device_id intel_rng_ids[] = {
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0x2410) }, /* AA */
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0x2420) }, /* AB */
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0x244c) }, /* BAM */
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0x248c) }, /* CAM */
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0x24cc) }, /* DBM */
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0x2641) }, /* FBM */
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0x27b9) }, /* GxM */
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0x27bd) }, /* GxM DH */
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0x2440) }, /* BA */
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0x2480) }, /* CA */
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0x24c0) }, /* DB */
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0x24d0) }, /* Ex */
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0x25a1) }, /* 6300 */
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0x2640) }, /* Fx */
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0x2670) }, /* 631x/632x */
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0x2671) }, /* 631x/632x */
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0x2672) }, /* 631x/632x */
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0x2673) }, /* 631x/632x */
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0x2674) }, /* 631x/632x */
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0x2675) }, /* 631x/632x */
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0x2676) }, /* 631x/632x */
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0x2677) }, /* 631x/632x */
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0x2678) }, /* 631x/632x */
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0x2679) }, /* 631x/632x */
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0x267a) }, /* 631x/632x */
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0x267b) }, /* 631x/632x */
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0x267c) }, /* 631x/632x */
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0x267d) }, /* 631x/632x */
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0x267e) }, /* 631x/632x */
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0x267f) }, /* 631x/632x */
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0x27b8) }, /* Gx */
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0x2450) }, /* E  */
    { }, /* NULL */
};

static state intel_rng_init(void)
{
    struct platform_device *pdev;
    struct resource *res;

    if (pci_find_device(intel_rng_ids, NULL)) {
        pdev = kzalloc(sizeof(*pdev) + sizeof(*res), GFP_KERNEL);
        if (!pdev)
            return -ENOMEM;

        res = (void *)pdev + sizeof(*pdev);
        res->start = INTEL_RNG_BASE;
        res->size = INTEL_RNG_SIZE;
        res->type = RESOURCE_MMIO;

        pdev->name = "intel,rng";
        pdev->resource = res;
        pdev->resources_nr = 1;
        platform_device_register(pdev);
    }

    return platform_driver_register(&intel_rng_driver);
}
driver_initcall(intel_rng_init);
