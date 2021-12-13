/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#define DRIVER_NAME "gx6605s-spi"

#include <kernel.h>
#include <initcall.h>
#include <driver/platform.h>
#include <driver/spi.h>
#include <driver/spi/gx6605s.h>
#include <asm/delay.h>
#include <asm/proc.h>
#include <asm/io.h>

struct gx6605s_device {
    struct spi_host host;
    spinlock_t lock;
    void *base;
};

#define host_to_gx6605s(spi) \
    container_of(spi, struct gx6605s_device, host)

static __always_inline uint32_t
gx6605s_read(struct gx6605s_device *gdev, unsigned int reg)
{
    return readl(gdev->base + reg);
}

static __always_inline void
gx6605s_write(struct gx6605s_device *gdev, unsigned int reg, uint32_t val)
{
    writel(gdev->base + reg, val);
}

static inline void spi_transfer_len(struct gx6605s_device *gdev, int len)
{
    uint32_t val;

    val = gx6605s_read(gdev, GX6605S_SPI_CTRL);
    val &= ~GX6605S_SPI_CTRL_ICNT_MASK;
    val |= ((len - 1) << 14) & GX6605S_SPI_CTRL_ICNT_MASK;
    gx6605s_write(gdev, GX6605S_SPI_CTRL, val);
}

static state spi_transfer_trigger(struct gx6605s_device *gdev, uint32_t trigger, uint32_t *read, uint32_t write)
{
    unsigned int timeout = 100;
    uint32_t val;

    gx6605s_write(gdev, GX6605S_SPI_TX_FIFO, write);
    val = gx6605s_read(gdev, GX6605S_SPI_CTRL);
    val |= trigger;
    gx6605s_write(gdev, GX6605S_SPI_CTRL, val);

    while (--timeout) {
        if (gx6605s_read(gdev, GX6605S_SPI_STAT) & GX6605S_SPI_STAT_OPE_RDY)
            break;
        udelay(10);
    }

    val = gx6605s_read(gdev, GX6605S_SPI_CTRL);
    val &= ~trigger;
    gx6605s_write(gdev, GX6605S_SPI_CTRL, val);
    *read = gx6605s_read(gdev, GX6605S_SPI_RX_FIFO);

    return timeout ? -ENOERR : -EBUSY;
}

static state gx6605s_spi_startup(struct spi_host *host)
{
    struct gx6605s_device *gdev = host_to_gx6605s(host);
    gx6605s_write(gdev, GX6605S_SPI_CTRL, 0x8420c000);
    return -ENOERR;
}

static void gx6605s_spi_shutdown(struct spi_host *host)
{
    struct gx6605s_device *gdev = host_to_gx6605s(host);
    gx6605s_write(gdev, GX6605S_SPI_CTRL, 0x00000000);
}

static state gx6605s_spi_transfer(struct spi_host *host, struct spi_device *spi, struct spi_transfer *tran)
{
    struct gx6605s_device *gdev = host_to_gx6605s(host);
    uint32_t val, xfer, count, len = tran->len;
    uint8_t *txbuf = tran->tx_buf, *rxbuf = tran->rx_buf;
    uint32_t trigger = GX6605S_SPI_CTRL_SPGO;
    state ret;

    spin_lock(&gdev->lock);

    /* Setup the transfer mode */
    val = gx6605s_read(gdev, GX6605S_SPI_CTRL);

    if (spi->mode & SPI_CPHA)
        val |= GX6605S_SPI_CTRL_CPHA;
    else
        val &= ~GX6605S_SPI_CTRL_CPHA;
    if (spi->mode & SPI_CPOL)
        val |= GX6605S_SPI_CTRL_CPOL;
    else
        val &= ~GX6605S_SPI_CTRL_CPOL;
    if (spi->mode & SPI_LSBF)
        val |= GX6605S_SPI_CTRL_LSB;
    else
        val &= ~GX6605S_SPI_CTRL_LSB;

    gx6605s_write(gdev, GX6605S_SPI_CTRL, val);

    /*
     * Dual read has two dummy clocks,
     * So we deal with it in advance.
     */
    if (tran->rx_lines == 2) {
        trigger |= GX6605S_SPI_CTRL_DUAL_READ;

        spi_transfer_len(gdev, 4);
        ret = spi_transfer_trigger(gdev, trigger, &val, 0);
        if (ret)
            goto exit;

        *rxbuf++ = val >> 8;
        *rxbuf++ = val >> 0;

        len -= 2;
    }

    /* Start normal transmission */
    for (; (xfer = min(len, 4)); len -= xfer) {
        spi_transfer_len(gdev, xfer);

        for (val = 0, count = xfer;
             tran->rx_lines != 2 && count;
             --count)
        {
            if (txbuf)
                val |= *txbuf++ << ((count - 1) * 8);
            else
                val |= 0xff << ((count - 1) * 8);
        }

        ret = spi_transfer_trigger(gdev, trigger, &val, val);
        if (ret)
            goto exit;

        for (count = xfer; count; --count)
            *rxbuf++ = val >> ((count - 1) * 8);
    }

exit:
    gx6605s_spi_startup(host);
    spin_unlock(&gdev->lock);
    return ret;
}

static state gx6605s_spi_devsel(struct spi_host *host, struct spi_device *dev, bool sel)
{
    struct gx6605s_device *gdev = host_to_gx6605s(host);
    uint32_t val;

    spin_lock(&gdev->lock);

    val = gx6605s_read(gdev, GX6605S_SPI_CTRL);
    val &= ~GX6605S_SPI_CTRL_CS_SEL;
    val |= (dev->dev_nr << 10) & GX6605S_SPI_CTRL_CS_SEL;

    if (sel == !!(dev->mode & SPI_CS_HI))
        val &= ~(GX6605S_SPI_CTRL_CS_EN | GX6605S_SPI_CTRL_CS_FORCE);
    else
        val |= GX6605S_SPI_CTRL_CS_EN | GX6605S_SPI_CTRL_CS_FORCE;

    gx6605s_write(gdev, GX6605S_SPI_CTRL, val);
    spin_unlock(&gdev->lock);
    return -ENOERR;
}

static struct spi_ops gx6605s_spi_ops = {
    .startup = gx6605s_spi_startup,
    .shutdown = gx6605s_spi_shutdown,
    .transfer = gx6605s_spi_transfer,
    .devsel = gx6605s_spi_devsel,
};

static state gx6605s_spi_probe(struct platform_device *pdev, const void *pdata)
{
    struct gx6605s_device *gdev;
    resource_size_t start, size;

    if (platform_resource_type(pdev, 0) != RESOURCE_MMIO)
        return -EINVAL;

    gdev = dev_kzalloc(&pdev->dev, sizeof(*gdev), GFP_KERNEL);
    if (!gdev)
        return -ENOMEM;
    platform_set_devdata(pdev, gdev);

    start = platform_resource_start(pdev, 0);
    size = platform_resource_size(pdev, 0);
    gdev->base = dev_ioremap(&pdev->dev, start, size);
    if (!gdev->base)
        return -ENOMEM;

    gdev->host.ops = &gx6605s_spi_ops;
    return spi_host_register(&gdev->host);
}

static struct dt_device_id gx6605s_spi_ids[] = {
    { .compatible = "nationalchip,gx6605s-spi" },
    { },  /* NULL */
};

static struct platform_driver gx6605s_spi_driver = {
    .driver = {
        .name = DRIVER_NAME,
    },
    .dt_table = gx6605s_spi_ids,
    .probe = gx6605s_spi_probe,
};

static state gx6605s_spi_init(void)
{
    return platform_driver_register(&gx6605s_spi_driver);
}
driver_initcall(gx6605s_spi_init);
