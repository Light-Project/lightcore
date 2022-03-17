/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#define DRIVER_NAME "ich-spi"
#define pr_fmt(fmt) DRIVER_NAME ": " fmt

#include <initcall.h>
#include <delay.h>
#include <ioops.h>
#include <printk.h>
#include <driver/platform.h>
#include <driver/mtd/spinor.h>
#include <driver/mtd/ich.h>

struct ich_device {
    struct spinor_device spinor;
    void *base;
};

#define spinor_to_idev(nor) \
    container_of(nor, struct ich_device, spinor)

static void ich_spi_dump_regs(struct ich_device *idev)
{
    unsigned int count;

    dev_debug(idev->spinor.device, "dump registers:\n");
    for (count = 0; count < ICH_SPI_FREG_NR; ++count)
        dev_debug(idev->spinor.device, "  FREG%d=0x%08x\n", count, readl(idev->base + ICH_SPI_FREG(count)));
    for (count = 0; count < ICH_SPI_FDATA_NR; ++count)
        dev_debug(idev->spinor.device, "  FDATA%d=0x%08x\n", count, readl(idev->base + ICH_SPI_FDATA(count)));

#if 0
    for (count = 0; count < ICH_SPI_FPR_NR; ++count)
        dev_debug(idev->spinor.device, "FPR%d=0x%08x\n", count, readl(idev->base + ICH_SPI_FPR(count)));
#endif

    dev_debug(idev->spinor.device, "  BFPR=0x%08x\n",       readl(idev->base + ICH_SPI_BFPR));
    dev_debug(idev->spinor.device, "  HSFS=0x%04x\n",       readw(idev->base + ICH_SPI_HSFS));
    dev_debug(idev->spinor.device, "  HSFC=0x%04x\n",       readw(idev->base + ICH_SPI_HSFC));
    dev_debug(idev->spinor.device, "  FADDR=0x%08x\n",      readl(idev->base + ICH_SPI_FADDR));
    dev_debug(idev->spinor.device, "  FRACC=0x%08x\n",      readl(idev->base + ICH_SPI_FRACC));
    dev_debug(idev->spinor.device, "  SSFS=0x%08x\n",       readw(idev->base + ICH_SPI_SSFS));
    dev_debug(idev->spinor.device, "  SSFC=0x%08x\n",       readw(idev->base + ICH_SPI_SSFC));
    dev_debug(idev->spinor.device, "  PREOP=0x%08x\n",      readw(idev->base + ICH_SPI_PREOP));
    dev_debug(idev->spinor.device, "  OPTYPE=0x%08x\n",     readw(idev->base + ICH_SPI_OPTYPE));
    dev_debug(idev->spinor.device, "  OPMENU=0x%016llx\n",  readq(idev->base + ICH_SPI_OPMENU));
    dev_debug(idev->spinor.device, "  FDOC=0x%08x\n",       readl(idev->base + ICH_SPI_FDOC));
    dev_debug(idev->spinor.device, "  FDOD=0x%08x\n",       readl(idev->base + ICH_SPI_FDOD));
}

static int intel_spi_protected(struct ich_device *idev, uint32_t base, uint32_t limit)
{
    unsigned int index;
    uint32_t val, pbase, plimit;

    for (index = 0; index < ICH_SPI_FPR_NR; ++index) {
        val = readl(idev->base + ICH_SPI_FPR(index));

        if (!(val & (ICH_SPI_FPR_RPE | ICH_SPI_FPR_WPE)))
            continue;

        pbase = (val & ICH_SPI_FPR_BASE) << 12;
        plimit = (val & ICH_SPI_FPR_LMT) >> 2;

        if (pbase >= base && plimit <= limit)
            return index;
    }

    return 0;
}

static inline state intel_spi_protect(struct ich_device *idev, uint32_t base, uint32_t limit)
{
    unsigned int index;
    uint32_t val;

    if (intel_spi_protected(idev, base, limit))
        return -EINVAL;

    for (index = 0; index < ICH_SPI_FPR_NR; ++index) {
        val = readl(idev->base + ICH_SPI_FPR(index));

        if (val & (ICH_SPI_FPR_RPE | ICH_SPI_FPR_WPE))
            continue;

        val = ICH_SPI_FPR_WPE;
        val |= (base >> 12) & ICH_SPI_FPR_BASE;
        val |= (limit << 2) & ICH_SPI_FPR_LMT;
        writel(idev->base + ICH_SPI_FPR(index), val);

        return -ENOERR;
    }

    return -EBUSY;
}

static inline void intel_spi_unprotect(struct ich_device *idev, uint32_t base, uint32_t limit)
{
    unsigned int index;
    uint32_t val;

    if (!(index = intel_spi_protected(idev, base, limit)))
        return;

    val = readl(idev->base + ICH_SPI_FPR(index));
    val &= ~(ICH_SPI_FPR_RPE | ICH_SPI_FPR_WPE);
    writel(idev->base + ICH_SPI_FPR(index), val);
}

static state ich_spi_wait_busy(struct ich_device *idev)
{
    unsigned int timeout = 500000;
    uint32_t val;

    while (--timeout) {
        val = readw(idev->base + ICH_SPI_HSFS);
        if (!(val & ICH_SPI_HSFS_SCIP))
            break;
        udelay(1);
    }

    return !!timeout;
}

static state ich_spi_read(struct spinor_device *nor, loff_t pos, void *buf, uint64_t len)
{
    struct ich_device *idev = spinor_to_idev(nor);
    uint32_t val, xfer = 1, count = 0;

    for (count = 0; xfer; pos += xfer, count += xfer, len -= xfer, buf += xfer) {
        xfer = min(len, ICH_SPI_BUFFER_SIZE);
        xfer = min(pos + xfer, round_up(pos + 1, SZ_4KiB)) - pos;

        writel(idev->base + ICH_SPI_FADDR, pos);
        val = readw(idev->base + ICH_SPI_HSFS);
        val |= ICH_SPI_HSFS_AEL | ICH_SPI_HSFS_FCERR | ICH_SPI_HSFS_DONE;
        writew(idev->base + ICH_SPI_HSFS, val);

        val = readw(idev->base + ICH_SPI_HSFC);
        val |= ((xfer - 1) << 8) | ICH_SPI_HSFC_FCYCLE_READ | ICH_SPI_HSFC_FGO;
        writew(idev->base + ICH_SPI_HSFC, val);

        if (!ich_spi_wait_busy(idev)) {
            dev_err(nor->device, "timeout while read cycle\n");
            return -EBUSY;
        }

        val = readw(idev->base + ICH_SPI_HSFS);
        if (val & ICH_SPI_HSFS_FCERR) {
            dev_err(nor->device, "read cycle error\n");
            return -EIO;
        } else if (val & ICH_SPI_HSFS_AEL) {
            dev_err(nor->device, "read access Error\n");
            return -EACCES;
        }

        memcpy_formio(buf, idev->base + ICH_SPI_FDATA_BASE, xfer);
    }

    return count;
}

static state ich_spi_write(struct spinor_device *nor, loff_t pos, void *buf, uint64_t len)
{
    struct ich_device *idev = spinor_to_idev(nor);
    uint32_t val, xfer = 1, count = 0;

    for (count = 0; xfer; pos += xfer, count += xfer, len -= xfer, buf += xfer) {
        xfer = min(len, ICH_SPI_BUFFER_SIZE);
        xfer = min(pos + xfer, round_up(pos + 1, SZ_4KiB)) - pos;

        memcpy_toio(idev->base + ICH_SPI_FDATA_BASE, buf, xfer);

        writel(idev->base + ICH_SPI_FADDR, pos);
        val = readw(idev->base + ICH_SPI_HSFS);
        val |= ICH_SPI_HSFS_AEL | ICH_SPI_HSFS_FCERR | ICH_SPI_HSFS_DONE;
        writew(idev->base + ICH_SPI_HSFS, val);

        val = readw(idev->base + ICH_SPI_HSFC);
        val |= ((xfer - 1) << 8) | ICH_SPI_HSFC_FCYCLE_WRITE | ICH_SPI_HSFC_FGO;
        writew(idev->base + ICH_SPI_HSFC, val);

        if (!ich_spi_wait_busy(idev)) {
            dev_err(nor->device, "timeout while read cycle\n");
            return -EBUSY;
        }

        val = readw(idev->base + ICH_SPI_HSFS);
        if (val & ICH_SPI_HSFS_FCERR) {
            dev_err(nor->device, "write cycle error\n");
            return -EIO;
        } else if (val & ICH_SPI_HSFS_AEL) {
            dev_err(nor->device, "write access Error\n");
            return -EACCES;
        }
    }

    return count;
};

static state ich_spi_erase(struct spinor_device *nor, loff_t pos, uint64_t len)
{
    struct ich_device *idev = spinor_to_idev(nor);
    uint32_t val;

    for (; len; pos += SZ_4KiB, len -= SZ_4KiB) {
        writel(idev->base + ICH_SPI_FADDR, pos);

        val = readw(idev->base + ICH_SPI_HSFS);
        val |= ICH_SPI_HSFS_AEL | ICH_SPI_HSFS_FCERR | ICH_SPI_HSFS_DONE;
        writew(idev->base + ICH_SPI_HSFS, val);

        val = readw(idev->base + ICH_SPI_HSFC);
        val |= ICH_SPI_HSFC_FCYCLE_ERASE | ICH_SPI_HSFC_FGO;
        writew(idev->base + ICH_SPI_HSFC, val);

        if (!ich_spi_wait_busy(idev))
            return -EBUSY;

        val = readw(idev->base + ICH_SPI_HSFS);
        if (val & ICH_SPI_HSFS_FCERR) {
            dev_err(nor->device, "erase cycle error\n");
            return -EIO;
        } else if (val & ICH_SPI_HSFS_AEL) {
            dev_err(nor->device, "erase access Error\n");
            return -EACCES;
        }
    }

    return -ENOERR;
}

static struct spinor_ops ich_spi_ops = {
    .read = ich_spi_read,
    .write = ich_spi_write,
    .erase = ich_spi_erase,
};

static void ich_spi_hwinit(struct ich_device *idev)
{
    uint32_t val;

    /* Disable #SMI generation from HW sequencer */
    val = readw(idev->base + ICH_SPI_HSFC);
    val &= ~ICH_SPI_HSFC_FSMIE;
    writew(idev->base + ICH_SPI_HSFC, val);
}

static state ich_spi_probe(struct platform_device *pdev, const void *pdata)
{
    struct ich_device *idev;
    resource_size_t base, size;

    base = platform_resource_start(pdev, 0);
    size = platform_resource_size(pdev, 0);

    idev = dev_kzalloc(&pdev->dev, sizeof(*idev), GFP_KERNEL);
    if (!idev)
        return -ENOMEM;

    idev->base = dev_ioremap(&pdev->dev, base, size);
    if (!idev->base)
        return -ENOMEM;

    idev->spinor.device = &pdev->dev;
    idev->spinor.ops = &ich_spi_ops;
    platform_set_devdata(pdev, idev);

    ich_spi_hwinit(idev);
    ich_spi_dump_regs(idev);
    return spinor_register(&idev->spinor);
}

static const struct platform_device_id ich_spi_ids[] = {
    { .name = INTEL_SPI_ICH_MATCH_ID },
    { }, /* NULL */
};

static struct platform_driver ich_spi_driver = {
    .driver = {
        .name = DRIVER_NAME,
    },
    .platform_table = ich_spi_ids,
    .probe = ich_spi_probe,
};

static state ich_spi_init(void)
{
    return platform_driver_register(&ich_spi_driver);
}
driver_initcall(ich_spi_init);
