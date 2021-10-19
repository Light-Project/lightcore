/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#define DRIVER_NAME "intel-spi"
#define pr_fmt(fmt) DRIVER_NAME ": " fmt

#include <string.h>
#include <initcall.h>
#include <driver/pci.h>
#include <driver/platform.h>
#include <driver/mtd/spinor.h>
#include <driver/mtd/intel.h>
#include <printk.h>

#include <asm/delay.h>
#include <asm/io.h>

struct intel_spec {
    unsigned int pr_base, pr_num;
    unsigned int ss_base;
    unsigned int freg_num;
    bool erase_64k;
};

struct intel_device {
    struct spinor_device spinor;
    const struct intel_spec *spec;
    void *base;
};

#define spinor_to_idev(nor) \
    container_of(nor, struct intel_device, spinor)

#define INTEL_SPI_TIMEOUT 5000

static __always_inline uint32_t
intel_read(struct intel_device *idev, unsigned int reg)
{
    return readl(idev->base + reg);
}

static __always_inline void
intel_write(struct intel_device *idev, unsigned int reg, uint32_t val)
{
    writel(idev->base + reg, val);
}

#define intel_pr_read(idev, index) \
    intel_read(idev, idev->spec->pr_base + INTEL_SPI_PR(index))

#define intel_pr_write(idev, index, val) \
    intel_write(idev, idev->spec->pr_base + INTEL_SPI_PR(index), val)

static bool intel_spi_wait_busy(struct intel_device *idev)
{
    unsigned int timeout = INTEL_SPI_TIMEOUT;
    uint32_t val;

    while (--timeout) {
        val = intel_read(idev, INTEL_SPI_HSCTL);
        if (!(val & INTEL_SPI_HSCTL_SCIP))
            break;
        udelay(1);
    }

    return !!timeout;
}

static int intel_spi_protected(struct intel_device *idev,
                    unsigned int base, unsigned int limit)
{
    unsigned int index;
    uint32_t val, pbase, plimit;

    for (index = 0; index < idev->spec->pr_num; ++index) {
        val = intel_pr_read(idev, index);

        if (!(val & (INTEL_SPI_PR_RPE | INTEL_SPI_PR_WPE)))
            continue;

        pbase = val & INTEL_SPI_PR_BASE;
        plimit = (val & INTEL_SPI_PR_LMT) >> 16;

        if (pbase >= base && plimit <= limit)
            return index + 1;
    }

    return 0;
}

static inline state intel_spi_protect(struct intel_device *idev,
                        unsigned int base, unsigned int limit)
{
    unsigned int index;
    uint32_t val;

    if (intel_spi_protected(idev, base, limit))
        return -EINVAL;

    for (index = 0; index < idev->spec->pr_num; ++index) {
        val = intel_pr_read(idev, index);

        if (val & (INTEL_SPI_PR_RPE | INTEL_SPI_PR_WPE))
            continue;

        val = INTEL_SPI_PR_RPE | INTEL_SPI_PR_WPE;
        val |= base & INTEL_SPI_PR_BASE;
        val |= (limit << 16) & INTEL_SPI_PR_LMT;
        intel_pr_write(idev, index, val);

        return index;
    }

    return -EBUSY;
}

static inline void intel_spi_unprotect(struct intel_device *idev,
                            unsigned int base, unsigned int limit)
{
    unsigned int index;

    if (!(index = intel_spi_protected(idev, base, limit)))
        return;

    intel_pr_write(idev, index - 1, 0);
}

static state intel_spi_receive(struct intel_device *idev, void *buf, uint32_t len)
{
    uint32_t xfer;
    int index;

    if (len > INTEL_SPI_BUFFER_SIZE)
        return -EINVAL;

    for (index = 0 ; (xfer = min(len, 4)); --len, ++index) {
        memcpy(buf, idev->base + INTEL_SPI_DATA(index), index);
        buf += xfer;
    }

    return -ENOERR;
}

static state intel_spi_read(struct spinor_device *nor, loff_t pos, void *buf, uint64_t len)
{
    struct intel_device *idev = spinor_to_idev(nor);
    uint32_t xfer, count, val;
    state ret;

    for (; ; len -= xfer, count += xfer, buf += xfer) {
        xfer = min(len, INTEL_SPI_BUFFER_SIZE);

        val = intel_read(idev, INTEL_SPI_HSCTL);
        val &= ~(INTEL_SPI_HSCTL_FDBC | INTEL_SPI_HSCTL_FCYCLE);
        val |= ((xfer - 1) << 24) | INTEL_SPI_HSCTL_READ | INTEL_SPI_HSCTL_FGO;
        intel_write(idev, INTEL_SPI_HSCTL, val);
        intel_write(idev, INTEL_SPI_ADDR, pos);

        if (!intel_spi_wait_busy(idev))
            return -EBUSY;

        val = intel_read(idev, INTEL_SPI_HSCTL);
        if (val & INTEL_SPI_HSCTL_FCERR)
            return -EIO;
        else if (val & INTEL_SPI_HSCTL_AEL)
            return -EACCES;

        if ((ret = intel_spi_receive(idev, buf, xfer)))
            return ret;
    }

    return count;
}

// static state intel_spi_write(struct spinor_device *nor)
// {
//     struct intel_device *idev = spinor_to_idev(nor);

//     return -ENOERR;
// }

// static state intel_spi_erase(struct spinor_device *nor)
// {
//     struct intel_device *idev = spinor_to_idev(nor);

//     return -ENOERR;
// }

static struct spinor_ops intel_spi_ops = {
    .read = intel_spi_read,
    // .write = intel_spi_write,
    // .erase = intel_spi_erase,
};

static state intel_spi_probe(struct intel_device *idev)
{
    char buff[32] = {};

    idev->spinor.ops = &intel_spi_ops;
    intel_spi_read(&idev->spinor, 0, buff, 32);

    for (int count = 0; count < 8; count++)
        printk(" 0x%x\n", buff[count]);

    return spinor_register(&idev->spinor);
}

static state intel_spi_platform_probe(struct platform_device *pdev, void *pdata)
{
    struct intel_device *idev;
    resource_size_t base, size;

    if (platform_resource_type(pdev, 0) != RESOURCE_MMIO)
        return -ENODEV;

    idev = dev_kzalloc(&pdev->dev, sizeof(*idev), GFP_KERNEL);
    if (!idev)
        return -ENOMEM;
    platform_set_devdata(pdev, idev);

    base = platform_resource_start(pdev, 0);
    size = platform_resource_size(pdev, 0);

    idev->base = dev_ioremap(&pdev->dev, base, size);
    if (idev->base)
        return -ENOMEM;

    idev->spec = pdata;
    return intel_spi_probe(idev);
}

static state intel_spi_pci_probe(struct pci_device *pdev, void *pdata)
{
    struct intel_device *idev;
    resource_size_t base, size;

    if (pci_resource_type(pdev, 0) != RESOURCE_MMIO)
        return -ENODEV;

    idev = dev_kzalloc(&pdev->dev, sizeof(*idev), GFP_KERNEL);
    if (!idev)
        return -ENOMEM;
    pci_set_devdata(pdev, idev);

    base = pci_resource_start(pdev, 0);
    size = pci_resource_size(pdev, 0);

    idev->base = dev_ioremap(&pdev->dev, base, size);
    if (idev->base)
        return -ENOMEM;

    idev->spec = pdata;
    return intel_spi_probe(idev);
}

static struct intel_spec intel_spi_byt = {
    .pr_base = 0x74, .pr_num = 5,
    .ss_base = 0x90,
    .erase_64k = false,
};

static struct intel_spec intel_spi_lpt = {
    .pr_base = 0x74, .pr_num = 5,
    .ss_base = 0xa0,
    .erase_64k = false,
};

static struct intel_spec intel_spi_bxt = {
    .pr_base = 0x84, .pr_num = 6,
    .ss_base = 0xa0,
    .erase_64k = true,
};

static struct intel_spec intel_spi_cnl = {
    .pr_base = 0x84, .pr_num = 5,
    .ss_base = 0,
    .erase_64k = false,
};

static const struct platform_device_id intel_spi_platform_ids[] = {
    { .name = INTEL_SPI_BYT_MATCH_ID, .data = &intel_spi_byt},
    { .name = INTEL_SPI_LPT_MATCH_ID, .data = &intel_spi_lpt},
    { .name = INTEL_SPI_BXT_MATCH_ID, .data = &intel_spi_bxt},
    { .name = INTEL_SPI_CNL_MATCH_ID, .data = &intel_spi_cnl},
    { }, /* NULL */
};

static const struct pci_device_id intel_spi_pci_ids[] = {
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0x02a4), .data = &intel_spi_bxt},
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0x06a4), .data = &intel_spi_bxt},
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0x18e0), .data = &intel_spi_bxt},
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0x19e0), .data = &intel_spi_bxt},
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0x1bca), .data = &intel_spi_bxt},
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0x34a4), .data = &intel_spi_bxt},
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0x43a4), .data = &intel_spi_cnl},
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0x4b24), .data = &intel_spi_bxt},
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0x4da4), .data = &intel_spi_bxt},
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0x51a4), .data = &intel_spi_cnl},
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0x54a4), .data = &intel_spi_cnl},
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0x7aa4), .data = &intel_spi_cnl},
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0xa0a4), .data = &intel_spi_bxt},
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0xa1a4), .data = &intel_spi_bxt},
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0xa224), .data = &intel_spi_bxt},
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0xa324), .data = &intel_spi_cnl},
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0xa3a4), .data = &intel_spi_bxt},
    { }, /* NULL */
};

static struct platform_driver intel_spi_platform_driver = {
    .driver = {
        .name = DRIVER_NAME,
    },
    .platform_table = intel_spi_platform_ids,
    .probe = intel_spi_platform_probe,
};

static struct pci_driver intel_spi_pci_driver = {
    .driver = {
        .name = DRIVER_NAME,
    },
    .id_table = intel_spi_pci_ids,
    .probe = intel_spi_pci_probe,
};

static state intel_spi_platform_init(void)
{
    return platform_driver_register(&intel_spi_platform_driver);
}
driver_initcall(intel_spi_platform_init);

static state intel_spi_pci_init(void)
{
    return pci_driver_register(&intel_spi_pci_driver);
}
driver_initcall(intel_spi_pci_init);
